export class Aula {
    public key: string;
    public edificio: string;
    public nombre: string;
    public nodoMac: string;
    public tiempoEntreLecturas: number;
    public comandoIR: string;

    constructor(key?:string, edificio?: string, nombre?: string, nodoMac?: string) {
        this.edificio = edificio || '';
        this.nombre = nombre || '';
        this.nodoMac = nodoMac || '';
    }
}