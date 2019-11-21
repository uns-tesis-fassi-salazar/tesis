export class Aula {
    public departamento: string;
    public nombre: string;
    public nodoMac: string;

    constructor(departamento?: string, nombre?: string, nodoMac?: string) {
        this.departamento = departamento || '';
        this.nombre = nombre || '';
        this.nodoMac = nodoMac || '';
    }

}