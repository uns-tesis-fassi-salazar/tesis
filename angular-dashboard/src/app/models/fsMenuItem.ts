import { NbMenuItem } from '@nebular/theme'

export class FSMenuItem extends NbMenuItem {
    public nodoMac: string;
    public aulaNombre: string;
    public edificio: string;

    constructor(nbMenuItem?: any){
        super();
        if(nbMenuItem) {
            this.nodoMac = nbMenuItem.nodoMac;
            this.aulaNombre = nbMenuItem.aulaNombre;
            this.edificio = nbMenuItem.edificio;
        }
    }
}