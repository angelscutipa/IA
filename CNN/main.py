# importamos la libreria
## -*- coding: utf-8 -*-
import tensorflow as tf
from tensorflow.python import control_flow_ops
import time, os
import pickle

import sys 
path = '/home/angelcs/Documentos/IA/FINAL/models-master/tutorials/image/cifar10' 
sys.path.append (path) 


import cifar10_input
#cifar10_input.maybe_download_and_extract()

# importamos librerias adicionales
import numpy as np
import matplotlib.pyplot as plt
import cPickle


# funciones de ayuda
def unpickle(file):
    fo = open(file, 'rb')
    dict = cPickle.load(fo)
    fo.close()
    data = dict['data']
    imgs = np.transpose(np.reshape(data,(-1,32,32,3), order='F'),
                        axes=(0,2,1,3)) #order batch,x,y,color
    y = np.asarray(dict['labels'], dtype='uint8')
    return y, imgs

clases = ['avion','auto','pajaro','gato','ciervo',
          'perro','rana','caballo','barco','camion']


# cargar los datos
labels, data = unpickle('cifar-10-batches-py/test_batch.bin')

# forma del dataset
print 'forma dataset prueba: {}'.format(data.shape)

# dibujar imagenes
fig = plt.figure(figsize=(7,7))
for i in range(25):
    fig.add_subplot(5,5,(i+1),xticks=[], yticks=[])
    plt.title("{}".format(clases[labels[i]]))
    plt.imshow(data[i])
plt.savefig('cifar.jpg')



# dibujar 200 imagenes
conj = np.zeros((320,640,3),dtype='uint8')
for i in range(10):
    idx = np.where(labels == i)[0]
    for j in range(20):
        conj[32*i:32*(i+1),32*j:32*(j+1),:] = data[idx[j]]

fig = plt.figure(figsize=(10,20))
imgs = plt.imshow(conj)


# armado de la red

# Parametros
ratio_aprendizaje = 0.01
eopcas_entrenamiento = 5
tam_lote = 128
pasos_visualizar = 500
NUM_EJEM_POR_EPOCA_ENTRENAMIENTO = 50000

def inputs(eval_data=True):
    data_dir = '/home/angelcs/Documentos/IA/FINAL/cifar-10-batches-py' 
    print "hola que tal "+data_dir
    return cifar10_input.inputs(eval_data=eval_data, data_dir=data_dir,
                              batch_size=tam_lote)

def distorted_inputs():
    data_dir = '/home/angelcs/Documentos/IA/FINAL/cifar-10-batches-py' 
    print "hola que tal "+data_dir
    return cifar10_input.distorted_inputs(data_dir=data_dir,
                                        batch_size=tam_lote)

def filter_summary(V, weight_shape):
    ix = weight_shape[0]
    iy = weight_shape[1]
    cx, cy = 8, 8
    V_T = tf.transpose(V, (3, 0, 1, 2))
    tf.summary.image("filters", V_T, max_outputs=64) 

#Polling
def max_pool(input, k=2):
    return tf.nn.max_pool(input, ksize=[1, k, k, 1],
                          strides=[1, k, k, 1], padding='SAME')

def loss(output, y):
    xentropy = tf.nn.softmax_cross_entropy_with_logits(output, tf.cast(y, tf.int64))  
    loss = tf.reduce_mean(xentropy)
    return loss

# aqui normalizamos los lotes de la red
def conv_batch_norm(x, n_out, phase_train):
    beta_init = tf.constant_initializer(value=0.0, dtype=tf.float32)
    gamma_init = tf.constant_initializer(value=1.0, dtype=tf.float32)

    beta = tf.get_variable("beta", [n_out], initializer=beta_init)
    gamma = tf.get_variable("gamma", [n_out], initializer=gamma_init)

    batch_mean, batch_var = tf.nn.moments(x, [0,1,2], name='moments')
    ema = tf.train.ExponentialMovingAverage(decay=0.9)
    ema_apply_op = ema.apply([batch_mean, batch_var])
    ema_mean, ema_var = ema.average(batch_mean), ema.average(batch_var)
    def mean_var_with_update():
        with tf.control_dependencies([ema_apply_op]):
            return tf.identity(batch_mean), tf.identity(batch_var)
    mean, var = control_flow_ops.cond(phase_train,
        mean_var_with_update,
        lambda: (ema_mean, ema_var))

    normed = tf.nn.batch_norm_with_global_normalization(x, mean, var,
        beta, gamma, 1e-3, True)
    return normed

def layer_batch_norm(x, n_out, phase_train):
    beta_init = tf.constant_initializer(value=0.0, dtype=tf.float32)
    gamma_init = tf.constant_initializer(value=1.0, dtype=tf.float32)

    beta = tf.get_variable("beta", [n_out], initializer=beta_init)
    gamma = tf.get_variable("gamma", [n_out], initializer=gamma_init)

    batch_mean, batch_var = tf.nn.moments(x, [0], name='moments')
    ema = tf.train.ExponentialMovingAverage(decay=0.9)
    ema_apply_op = ema.apply([batch_mean, batch_var])
    ema_mean, ema_var = ema.average(batch_mean), ema.average(batch_var)
    def mean_var_with_update():
        with tf.control_dependencies([ema_apply_op]):
            return tf.identity(batch_mean), tf.identity(batch_var)
    mean, var = control_flow_ops.cond(phase_train,
        mean_var_with_update,
        lambda: (ema_mean, ema_var))

    reshaped_x = tf.reshape(x, [-1, 1, 1, n_out])
    normed = tf.nn.batch_norm_with_global_normalization(reshaped_x, mean, var,
        beta, gamma, 1e-3, True)
    return tf.reshape(normed, [-1, n_out])
    
#creamos la capa convolucional
def conv2d(input, weight_shape, bias_shape, phase_train, visualize=False):
    incoming = weight_shape[0] * weight_shape[1] * weight_shape[2]
    weight_init = tf.random_normal_initializer(stddev=(2.0/incoming)**0.5)
    W = tf.get_variable("W", weight_shape, initializer=weight_init)
    if visualize:
        filter_summary(W, weight_shape)
    bias_init = tf.constant_initializer(value=0)
    b = tf.get_variable("b", bias_shape, initializer=bias_init)
    logits = tf.nn.bias_add(tf.nn.conv2d(input, W,
                        strides=[1, 1, 1, 1], padding='SAME'), b)
    return tf.nn.relu(conv_batch_norm(logits, weight_shape[3],
                                      phase_train))

#construye cada capa
def layer(input, weight_shape, bias_shape, phase_train):
    weight_init = tf.random_normal_initializer(stddev=
                                               (2.0/weight_shape[0])**0.5)
    bias_init = tf.constant_initializer(value=0)
    W = tf.get_variable("W", weight_shape,
                        initializer=weight_init)
    b = tf.get_variable("b", bias_shape,
                        initializer=bias_init)
    logits = tf.matmul(input, W) + b
    return tf.nn.relu(layer_batch_norm(logits, weight_shape[1],
                                       phase_train))


#
def inference(x, keep_prob, phase_train):
    
    with tf.variable_scope("conv_1"):
        conv_1 = conv2d(x, [5, 5, 3, 64], [64], phase_train, visualize=True)
        pool_1 = max_pool(conv_1)
        
    with tf.variable_scope("conv_2"):
        conv_2 = conv2d(pool_1, [5, 5, 64, 64], [64], phase_train)
        pool_2 = max_pool(conv_2)
        
    with tf.variable_scope("fc_1"):
        dim = 1
        for d in pool_2.get_shape()[1:].as_list():
            dim *= d
        pool_2_flat = tf.reshape(pool_2, [-1, dim])
        fc_1 = layer(pool_2_flat, [dim, 384], [384], phase_train)
        # apply dropout
        fc_1_drop = tf.nn.dropout(fc_1, keep_prob)
        
    with tf.variable_scope("fc_2"):
        fc_2 = layer(fc_1_drop, [384, 192], [192], phase_train)
        # apply dropout
        fc_2_drop = tf.nn.dropout(fc_2, keep_prob)
        
    with tf.variable_scope("output"):
        output = layer(fc_2_drop, [192, 10], [10], phase_train)
    
    return output



def evaluate(output, y):
    correct_prediction = tf.equal(tf.cast(tf.argmax(output, 1), 
                                          dtype=tf.int32), y)
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    tf.summary.scalar("validation error", (1.0 - accuracy))
    return accuracy

def training(cost, global_step):
    tf.summary.scalar("cost", cost)
    optimizer = tf.train.AdamOptimizer(ratio_aprendizaje)
    train_op = optimizer.minimize(cost, global_step=global_step)
    return train_op



# Lanzando el modelo
with tf.Graph().as_default():
    with tf.variable_scope("cifar_Conv_model"):
        x = tf.placeholder("float", [None, 24, 24, 3])
        y = tf.placeholder("int32", [None])
        keep_prob = tf.placeholder(tf.float32) 
        phase_train = tf.placeholder(tf.bool) # entrenamiento

        distorted_images, distorted_labels = distorted_inputs()
        val_images, val_labels = inputs()

        output = inference(x, keep_prob, phase_train)

        cost=tf.reduce_mean(output)
        global_step = tf.Variable(0, name='global_step', trainable=False)

        train_op = training(cost, global_step)

        eval_op = evaluate(output, y)

        summary_op = tf.summary.merge_all()

        saver = tf.train.Saver()

        sess = tf.Session()

        summary_writer = tf.summary.FileWriter("conv_cifar_bn_logs/",
                                                graph=sess.graph)

              
        init_op = tf.initialize_all_variables()

        sess.run(init_op)

        tf.train.start_queue_runners(sess=sess)

        # ciclo de entrenamiento
        for epoch in range(eopcas_entrenamiento):
            avg_cost = 0.
            total_lotes = int(NUM_EJEM_POR_EPOCA_ENTRENAMIENTO/tam_lote)
            # ciclos para cada lote
            for i in range(total_lotes):
                # entrenamiento utilisando cada lote
                train_x, train_y = sess.run([distorted_images, distorted_labels])
                
                _, new_cost = sess.run([train_op, cost], 
                                       feed_dict={x: train_x, 
                                                  y: train_y,
                                                  keep_prob: 1,
                                                  phase_train: True})
                # promedio de perdida
                avg_cost += new_cost/total_lotes
                print "Epoca %d, minilote %d de %d. Costo = %0.4f." %(epoch, 
                                                    i, total_lotes, new_cost)
                    
            # mostrar registros por paso de epoca
            if epoch % pasos_visualizar == 0:
                print "Epoca:", '%04d' % (epoch+1), "Costo =", "{:.9f}".format(avg_cost)
                
                val_x, val_y = sess.run([val_images, val_labels])
                
                accuracy = sess.run(eval_op, feed_dict={x: val_x, 
                                                        y: val_y, 
                                                        keep_prob: 1,
                                                        phase_train: False})
                
                print "Error de Validacion:", (1 - accuracy)
                
                summary_str = sess.run(summary_op, feed_dict={x: train_x,
                                                              y: train_y,
                                                              keep_prob: 1,
                                                              phase_train: False})
                summary_writer.add_summary(summary_str, sess.run(global_step))
                
                saver.save(sess, "conv_cifar_bn_logs/model-checkpoint",
                           global_step=global_step)

        print "Optimizacion Finalizada!"
        # validation first 256 imgs
        val_x, val_y = sess.run([val_images, val_labels])
        accuracy = sess.run(eval_op, feed_dict={x: val_x[:256], 
                                                y: val_y[:256], 
                                                keep_prob: 1, 
                                                phase_train: False})

        print "Test de precision:", accuracy

test2X=[[1.00, 0.28, 0.58, 0.14, 0.09, 0.05, 0.00, 0.19, 0.02, 0.00],
     [0.58, 0.70, 0.63, 0.12, 0.00, 0.19, 0.14, 0.00, 0.00, 0.00]]
test2Y=[[1.00, 0.70, 0.84, 0.56, 0.42, 0.28, 0.00, 0.00, 0.14, 0.00, 0.00, 0.00, 0.00],
     [1.00, 0.56, 0.84, 0.42, 0.70, 0.28, 0.00, 0.00, 0.14, 0.00, 0.00, 0.00, 0.00]]

results=model.predict(test2X)##para probar la data xoxo
print (results)

print (test2Y)