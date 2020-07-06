#----------- Importacion de modulos --------------#
import paho.mqtt.client as mqtt
import time
#--- Modulos locales ---#
from   data_sql import data, horario
from Hour import bloque, Led_asig, Fecha
#---------- Declaraciones ------------------------#
server = '192.168.43.214'  # IP Pc
port_ = 5050  # port broker
client_id_ = 'ciexpro_03'
#--- Topics ----------#
AA = 'AA'
AB = 'AB'
AC = 'AC'
general = 'general'
#---------------------#
#//////////////////////// Protocolo de comandos ////////////////////////////#
#______________ Funcion para comando cambiar ______________________#
def find_comm(msn):                                          # el comando sera de la forma (change_full_day,AC,Ju,A4,B)
    #------ Aula -----#
    Aula = msn[16:18]
    #----dias-----#
    if   msn[19] == 'L': d = 'Lunes'
    elif msn[19:21] == 'Ma': d = 'Martes'
    elif msn[19:21] == 'Mi': d = 'Miercoles'
    elif msn[19] == 'J': d = 'Jueves'
    elif msn[19] == 'V': d = 'Viernes'
    #-----Asigs-------#
    if   msn[23] == '1': a = 'Asignatura 1'
    elif msn[23] == '2': a = 'Asignatura 2'
    elif msn[23] == '3': a = 'Asignatura 3'
    elif msn[23] == '4': a = 'Asignatura 4'
    elif msn[23] == '5': a = 'Asignatura 5'
    elif msn[23] == '6': a = 'Asignatura 6'
    elif msn[23] == 'N': a = 'NULL'
    #---- Bloque ----#
    b = msn[25]
    #data.cursor.execute(data.select_table(Aula))
    data.cursor.execute(data.change(Aula, d, a, b))
    data.db.commit()
    client.publish('%s' %Aula, '\n\rCambio realizado en el horario del Aula %s' %Aula)
    return Aula, d, a, b
#_______________ Publicar todo el horario _________________________#
def pub_comm(msn):                                           # el comando sera de la forma (publish_all_asigs_for_auls,AA)
    Aula = msn[27:29]
    data.cursor.execute(data.select_table(Aula))
    obten = data.cursor.fetchall()
    for register in obten:  # iteracion sobre db
        Bloque = register[0]
        Lunes = register[1]
        Martes = register[2]
        Miercoles = register[3]
        Jueves = register[4]
        Viernes = register[5]
        client.publish('%s'%Aula, '\nBloque: %s--> Lunes: %s | Martes: %s | Miercoles: %s | Jueves: %s | Viernes: %s' %(Bloque, Lunes, Martes, Miercoles, Jueves, Viernes))
        client.publish('%s'%Aula, '_______________________________________________________________________________________________________________________')
    return Aula
#_________ Consultar bloque en funcion de asignatura ______________#
def consul_comm(msn):                                        # el comando sera de la forma (select_from_hori,AA,Asignatura 1,Lu)
    #---- Aula ----#
    Aula = msn[17:19]
    #---- Asignatura ---#
    asig = msn[20:32]
    #---- Dia -----#
    if   msn[33]    == 'L':  d = 'Lunes'
    elif msn[33:35] == 'Ma': d = 'Martes'
    elif msn[33:35] == 'Mi': d = 'Miercoles'
    elif msn[33]    == 'J':  d = 'Jueves'
    elif msn[33]    == 'V':  d = 'Viernes'
    #---------------#
    data.cursor.execute(data.select_asig(Aula, asig, d))
    ok = data.cursor.fetchall()
    for datos in ok:
        bloque = ok[0]
        client.publish( '%s'%Aula, '\nDia: %s\nAsignatura: %s\nBloque: %s\n' %(d, asig, bloque) )
    return Aula, asig, d, bloque
#_________ Consultar un solo dia de clase de aula especifica ______#
def one_diax(msn):                                           # el comando sera de la forma (publish_day,AA,Lu)
    Aula = msn[12:14]
    #---- Dia -----#
    if   msn[15] == 'L': d = 'Lunes'
    elif msn[15:17] == 'Ma': d = 'Martes'
    elif msn[15:17] == 'Mi': d = 'Miercoles'
    elif msn[15] == 'J': d = 'Jueves'
    elif msn[15] == 'V': d = 'Viernes'
    #--------------#
    data.cursor.execute(data.select_table(Aula))
    onex = data.cursor.fetchall()
    for day in onex:
        Bloque = onex[0]  # Bloque
        if d == 'Lunes': dia = day[1]
        elif d == 'Martes': dia = day[2]
        elif d == 'Miercoles': dia = day[3]
        elif d == 'Jueves': dia = day[4]
        elif d == 'Viernes': dia = day[5]
        client.publish('%s'%Aula, '--> %s: %s' %(d, dia))
#________ Borrar config de un dia x en aula x _____________________#
def delete_config_day(msn):                                  # el comando sera de la forma (delete_day_config,AA,Lu)
    Aula = msn[18:20]
    #---- Dia -----#
    if   msn[21] == 'L': d = 'Lunes'
    elif msn[21:23] == 'Ma': d = 'Martes'
    elif msn[21:23] == 'Mi': d = 'Miercoles'
    elif msn[21] == 'J': d = 'Jueves'
    elif msn[21] == 'V': d = 'Viernes'
    #---------------#
    data.cursor.execute(data.null_day(Aula, d))
    data.db.commit()
    client.publish('%s'%Aula, '\nSe ha borrado la configuracion del dia %s del aula %s'%(d, Aula))
#________ Borrar config de toda la semana de un aula x ____________#
def delete_config_week(msn):                                 # el comando sera de la forma (horis_zero,AA)
    Aula = msn[11:13]
    data.cursor.execute(data.null_week(Aula))
    data.db.commit()
    client.publish('%s'%Aula, '\nSe ha borrado la configuracion del Aula %s'%Aula)
    return Aula
#________ Dia Feriado para Aula especifica ________________________#
def free_dayz(msn):                                          # el comando sera de la forma (free_day,AA,Lu)
    Aula = msn[9:11]
    #---- Dia -----#
    if   msn[12] == 'L': d = 'Lunes'
    elif msn[12:14] == 'Ma': d = 'Martes'
    elif msn[12:14] == 'Mi': d = 'Miercoles'
    elif msn[12] == 'J': d = 'Jueves'
    elif msn[12] == 'V': d = 'Viernes'
    #---------------#
    data.cursor.execute(data.feri_day(Aula, d))
    data.db.commit()
    client.publish('%s'%Aula, '\nEl dia %s ahora es feriado'%d)
#_______ Eliminar una asignatura de un dia ________________________#
def one_asig_del(msn):                                       # el comando sera de la forma (delete_asig,AA,Lu,A1)
    Aula = msn[12:14]
    #----dias-----#
    if   msn[15] == 'L': d = 'Lunes'
    elif msn[15:17] == 'Ma': d = 'Martes'
    elif msn[15:17] == 'Mi': d = 'Miercoles'
    elif msn[15] == 'J': d = 'Jueves'
    elif msn[15] == 'V': d = 'Viernes'
    #-----Asigs-------#
    if   msn[19] == '1': a = 'Asignatura 1'
    elif msn[19] == '2': a = 'Asignatura 2'
    elif msn[19] == '3': a = 'Asignatura 3'
    elif msn[19] == '4': a = 'Asignatura 4'
    elif msn[19] == '5': a = 'Asignatura 5'
    data.cursor.execute(data.delte_asig(Aula, d, a))
    data.db.commit()
    client.publish('%s'%Aula, '\nSe ha eliminado la %s del Aula %s el dia %s'%(a, Aula, d))
#------------------------------------------------------------------#
def comm_help():
    client.publish(general, '''Los comandos que se pueden implementar son:\n\r
                                    ~ change_full_day -> para cambiar la asignatura de un dia y aula especificos, ejemplo:
                                        change_full_day,AC,Ju,A4,B.
                                        ejemplo de la sintaxis de las abreviaturas:
                                        AA -> Aula A (AA, AB, AC)
                                        A4 -> Asignatura 4 (desde A1 hasta A6)
                                        AN = Asignatura NULL
                                        Ju -> Jueves
                                        B -> Bloque B (desde A hasta)\n\r
                                    ~ publish_all_asigs_for_auls -> para publicar todo el horario de un Aula especifica, ejemplo:
                                        publish_all_asigs_for_auls,AA\n\r
                                    ~ select_from_hori -> Seleccionar una asignatura especifica y dia para mostrar sus bloques, ejemplo:
                                        select_from_hori,AA,Asignatura 1,Lu\n\r
                                    ~ publish_day -> Para publicar un dia completo de clase para un aula especifica, ejemplo:
                                        publish_day,AA,Lu\n\r
                                    ~ delete_day_config -> para borrar la configuracion de un dia y aula especificos, ejemplo:
                                        delete_day_config,AA,Lu\n\r
                                    ~ horis_zero -> Para borrar configuracion de una semana de un aula especifica, ejemplo:
                                        horis_zero,AA\n\r
                                    ~ free_day -> Para insertar un dia feriado, ejemplo:
                                        free_day,AA,Lu\n\r
                                    ~ delete_asig -> Para borrar asignatura de dia y aula especificos, ejemplo:
                                        delete_asig,AA,Lu,A1\n\r''')
#------------------------------------------------------------------#
#///////////////////////////////////////////////////////////////////////////#
#///////////////////////////////////////////////////////////////////////////#
#______________ Funciones conexion MQTT __________________________#
def conexion(client, userdata, flags, rc):
    print('Conexion establecida (%s)' % client._client_id)
    client.subscribe(general, qos =2)
    client.subscribe(AA, qos =2)
    client.subscribe(AB, qos =2)
    client.subscribe(AC, qos =2)
#----------------------------#
def mensaje(client, userdata, message):
    msg = str(message.payload.decode("utf-8"))
    print(str(message.topic),msg)
    #-------- Comandos ------------#
    if   msg[0:11] == 'publish_day':
        one_diax(msg)
    elif msg[0:15] == 'change_full_day':
        find_comm(msg)
    elif msg[0:26] == 'publish_all_asigs_for_auls':
        pub_comm(msg)
    elif msg[0:16] == 'select_from_hori':
        consul_comm(msg)
    elif msg[0:17] == 'delete_day_config':
        delete_config_day(msg)
    elif msg[0:10] == 'horis_zero':
        delete_config_week(msg)
    elif msg[0:8] == 'free_day':
        free_dayz(msg)
    elif msg[0:11] == 'delete_asig':
        one_asig_del(msg)
    elif msg == 'help':
        comm_help()
#----------------------------#
client = mqtt.Client(client_id = client_id_, clean_session = False)
client.on_connect = conexion
client.on_message = mensaje
client.connect(server, port_)
client.loop_start()
#__________________________________________________________#
block, dia = bloque()  # almacenar el bloque de horario actual
state_A = Led_asig('AA', dia, block)
state_B = Led_asig('AB', dia, block)
state_C = Led_asig('AC', dia, block)
while True:
    time.sleep(10)  # publicar cada 10 seg fecha y hora
    client.publish(general, Fecha)
    time.sleep(60)  # publicar cada min estado de iluminacion
    client.publish(AA, state_A)
    client.publish(AB, state_B)
    client.publish(AC, state_C)
