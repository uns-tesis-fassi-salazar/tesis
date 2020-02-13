import { Injectable } from '@angular/core';
import { AngularFireDatabase, AngularFireList } from '@angular/fire/database';
import { Observable } from 'rxjs';
import { NbToastrService, NbGlobalPosition, NbGlobalPhysicalPosition } from '@nebular/theme';
import { Log } from '../models';

@Injectable({
    providedIn: 'root'
})
export class LogService {

    constructor(
        private db: AngularFireDatabase,
    ) { }

    getLogsByMac(mac: string) {
        return this.db.list<Log>('logs/' + mac, ref => ref.orderByChild('secuencial')).valueChanges();
    }

}
