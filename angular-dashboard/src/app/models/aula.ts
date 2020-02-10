export class Aula {
    public key: string;
    public edificio: string;
    public nombre: string;
    public nodoMac: string;
    public comandoIR: string;
    public intervaloLecturas: number;
    public timeoutAulaVacia: number;
    public horaInicioAuto: string;
    public horaFinAuto: string;

    constructor(key?: string, edificio?: string, nombre?: string, nodoMac?: string, comandoIR?: string) {
        this.edificio = edificio || '';
        this.nombre = nombre || '';
        this.nodoMac = nodoMac || '';
        this.comandoIR = comandoIR || '';
        this.intervaloLecturas = 5;
        this.timeoutAulaVacia = 10;
        this.horaInicioAuto = '23:00';
        this.horaFinAuto = '05:00';
    }
}