#----------- Importacion de modulos --------------#
from datetime import datetime
from   data_sql import data, horario
import time
#-------------------------------------------------#
now = datetime.now()
hora = now.hour
Fecha = '{:%d de %b / %H:%M:%S}'.format(now)
dia = '{:%a}'.format(now)  # dia
#-------------------------------------------------#
#------------- Led on/off msg --------------------#
def Led_asig(Aula, dia, block):  # db of Aula('AA'...), dia(Lunes...), block('A','B'...), 
    data.cursor.execute(data.select_hora(Aula, block))
    clase = horario()[dia]
    if clase != None and clase != 'Feriado':
        state = 'on'
    else:
        state = 'off'
    print(state)
    return state
#------------ Block msg --------------------------#
bloc = 0
def bloque():
    dia = '{:%a}'.format(now)  # dia
    global bloc
    if hora in range(7, 9):
        bloc = 'A'
    elif hora in range (9, 11):
        bloc = 'B'
    elif hora in range(11, 13):
        bloc = 'C'
    elif hora in range(14, 16):
        bloc = 'D'
    elif hora in range(16, 18):
        bloc = 'E'
    #-------------------------#
    if   dia == 'Mon': dayx = 1
    elif dia == 'Tue': dayx = 2
    elif dia == 'Wed': dayx = 3
    elif dia == 'Thu': dayx = 4
    elif dia == 'Fri': dayx = 5
    #-------------------------#
    return bloc, dayx
#-------------------------------------------------#
# print(Fecha)
# print(dia)
