export class ComandoIR {
    public key: string;
    public marca: string;
    public modelo: string;

    constructor(comando: ComandoIR) {
        this.marca = comando.marca;
        this.modelo = comando.modelo;
    }

}