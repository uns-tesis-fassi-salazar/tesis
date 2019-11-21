import { Injectable } from '@angular/core';
import { FiredbService } from './firedb.service';
import { Observable, BehaviorSubject } from 'rxjs';
import { Aula } from '../models/aula';
import { NbMenuService } from '@nebular/theme';
import { FSMenuItem } from '../models/fsMenuItem';

@Injectable({
    providedIn: 'root'
})
export class AulaService {

    public aulaSource: BehaviorSubject<Aula>;
    public aulaData: Observable<Aula>;

    constructor(private nbMenuService: NbMenuService) {
        this.aulaSource = new BehaviorSubject<Aula>(new Aula());
        this.aulaData = this.aulaSource.asObservable();
    }

    public get currentAula() {
        return this.aulaSource.value;
    }

    updateAula(newAula: Aula) {
        this.aulaSource.next(newAula);
    }




}
