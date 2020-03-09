export class DBConstants{
    // EDIFICIOS
    public static readonly edificios = 'edificios/';
    
    // AULAS
    public static readonly aulas = 'aulas/';
    public static readonly aulaComandoIR = 'comandoIR/';
    public static readonly aulaEdificio = 'edificio/';
    
    //NODOS
    public static readonly nodos = 'nodos/';
    public static readonly nodoAulaAsignada = 'aulaAsignada';
    public static readonly nodoFirmwareVersion = 'firmwareVersion';
    public static readonly nodoConfigTiempoEntreLectura = 'tiempoEntreLecturas';
    // ACTUADORES
    public static readonly nodoComando = 'comando';
    // SENSORES
    public static readonly nodoSensores = 'sensores/';
    public static readonly nodoSensorHumedad = 'humedad';
    public static readonly nodoSensorLuminosidad = 'luminocidad';
    public static readonly nodoSensorMovimiento = 'movimiento';
    public static readonly nodoSensorTemperatura = 'temperatura';
    public static readonly nodoSensorHall = 'corriente';
    // CODIGO COMANDOS
    public static readonly comandoReset = 0;
    public static readonly comandoEmitirIR = 1;
    public static readonly comandoGrabarIR = 2;
    public static readonly comandoActualizarIR = 3;
    public static readonly comandoEncenderLuces = 4;
    public static readonly comandoApagarLuces = 5;
    public static readonly comandoActualizarFirmware = 6;
    public static readonly comandoActualizarAula = 7;

    // IR
    public static readonly comandosIR = 'comandosIR/';

}
