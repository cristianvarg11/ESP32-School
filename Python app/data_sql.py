#------------------ Importacion de modulos ------------------------#
import pymysql
#------------------------------------------------------------------#
class Data:
    def __init__(self):  # inicializar base de datos
        self.db = pymysql.connect(host = 'localhost', user = 'root', password = 'penny11042002', db = 'final')
        self.cursor = self.db.cursor()
    #______________________________________________________________#
    def create_table(self, nombre):  # creacion de la tabla
        self.create = "CREATE TABLE {} (Bloques CHAR(10), Lunes CHAR (30), Martes CHAR (30), Miercoles CHAR (30), Jueves CHAR(30), Viernes CHAR(30))".format(nombre)
        return self.create
    #______________________________________________________________#
    def select_table(self,nombre_tabla):  # seleccion de tabla completa
        self.table = "select * from {} ".format(nombre_tabla)
        return self.table
    #______________________________________________________________#
    def select_hora(self,nombre_tabla, bloque):  # seleccionar bloque de hora especifico
        self.tabla = '''select * from {} where Bloques = '{}' '''.format(nombre_tabla, bloque)
        return self.tabla
    #______________________________________________________________#
    def insert_asig_one_day(self,tabla, columna, as1, as2, as3, as4, as5):  # horario para un dia
        self.table = "insert into {}({}) VALUES('{}'),('{}'),('{}'),('{}'),('{}') ".format(tabla,columna,as1,as2,as3,as4,as5)
        return self.table
    #______________________________________________________________#
    def change(self,tabla,dia,new,bloque):  # cambiar una asignatura en un bloque y dia especifico
        self.actu = "UPDATE {} SET {} = '{}' where Bloques = '{}'".format(tabla,dia,new,bloque)
        return self.actu
    #______________________________________________________________#
    def delete(self,tabla):  # borrar tabla completa
        self.sup = "DELETE from {}".format(tabla)
        return self.sup
    #______________________________________________________________#
    def select_asig(self, tabla, asignatura, dia):  # seleccionar una asignatura especifica para mostrar sus bloques
        self.sel_day_asi = " select Bloques from {} where {} = '{}' ".format(tabla, dia, asignatura)
        return self.sel_day_asi
    #______________________________________________________________#
    def null_day(self,tabla, dia):  # eliminar configuracion de un dia de clase
        self.null = " update {} set {} = NULL ".format(tabla, dia)
        return self.null
    #______________________________________________________________#
    def null_week(self,tabla):  # eliminar configuracion de una semana de un aula especifica
        self.week = " update {} set Lunes = NULL, Martes = NULL, Miercoles = NULL, Jueves = NULL, Viernes = NULL ".format(tabla)
        return self.week
    #______________________________________________________________#
    def feri_day(self, tabla, dia):  # insertar un dia feriado
        self.free = " update {} set {} = 'Feriado' ".format(tabla, dia)
        return self.free
    #______________________________________________________________#
    def delte_asig(self,tabla,dia,asig):  # borrar asignatura de dia y aula especificos 
        self.asig = " update {} set {}  = NULL where {} = '{}' ".format(tabla, dia, dia, asig)
        return self.asig
#------------------------------------------------------------------#
data = Data()  # instancia
#------------------------------------------------------------------#
 #------------------------------------------------------------------#
def horario():  # funcion para obtener datos, antes, se debe ejecutar select_table('name') para elegir la tabla
    obten1 = data.cursor.fetchall()
    for register in obten1:  # iteracion sobre db
        Bloque = register[0]
        Lunes = register[1]
        Martes = register[2]
        Miercoles = register[3]
        Jueves = register[4]
        Viernes = register[5]
        return Bloque, Lunes, Martes, Miercoles, Jueves, Viernes
#------------------------------------------------------------------#