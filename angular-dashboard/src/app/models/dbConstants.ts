export class DBConstants{
    // CONFIGURACION
    public static get configTiempoVacia(): string { return 'TiempoVacia'; }
    public static get configTiempoEntreLectura(): string { return 'TiempoEntreLecturas'; }

    // ACTUADORES
    public static get actuadorAireId(): string { return 'Aire'; }
    public static get actuadorLucesId(): string { return 'Luces'; }
    public static get actuadorProyectorId(): string { return 'Proyector'; }
    public static get actuadorLedId(): string { return 'Led'; }

    // SENSORES
    public static get sensorHumedadId(): string { return 'Humedad'; }
    public static get sensorLuminocidadId(): string { return 'Luminocidad'; }
    public static get sensorMovimientoId(): string { return 'Movimiento'; }
    public static get sensorTemperaturaId(): string { return 'Temperatura'; }
}
