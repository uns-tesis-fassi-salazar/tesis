import { Injectable } from '@angular/core';
import { FiredbService } from './firedb.service';

@Injectable({
    providedIn: 'root'
})
export class AulasService {

    public departamentos: any;

    constructor(
        private firedbService: FiredbService
    ) {
        firedbService.getAulas().subscribe( obj => {
            console.log(obj);
            this.departamentos = obj;
        });
    }


}
