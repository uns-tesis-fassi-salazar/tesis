export class Edificio {
    public key: string;
    public nombre: string;

    constructor(nombre?: string) {
        this.nombre = nombre || '';
    }

}