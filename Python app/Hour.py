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
    return state, Aula
#------------ Block msg --------------------------#
def bloque():
    dia = '{:%a}'.format(now)  # dia
    if hora in range(7, 9):
        blo = 'A'
    elif hora in range (9, 11):
        blo = 'B'
    elif hora in range(11, 13):
        blo = 'C'
    elif hora in range(14, 16):
        blo = 'D'
    elif hora in range(16, 18):
        blo = 'E'
    #-------------------------#
    if   dia == 'Mon': dia = 1
    elif dia == 'Tue': dia = 2
    elif dia == 'Wed': dia = 3
    elif dia == 'Thu': dia = 4
    elif dia == 'Fri': dia = 5
    #-------------------------#
    return blo, dia
#-------------------------------------------------#
# print(Fecha)
# print(dia)