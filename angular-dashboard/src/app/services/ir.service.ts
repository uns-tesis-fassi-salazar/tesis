import { Injectable } from '@angular/core';
import { AngularFireDatabase, AngularFireList } from '@angular/fire/database';
import { Observable } from 'rxjs';
import { NbToastrService, NbGlobalPosition, NbGlobalPhysicalPosition } from '@nebular/theme';
import { FirebaseDatabase } from '@angular/fire';
import { DBConstants } from '../helpers';
import { map } from 'rxjs/operators';
import { ComandoIR } from '../models';

@Injectable({
  providedIn: 'root'
})
export class IRService {

  constructor(
    private db: AngularFireDatabase,
  ) { }

  getComandosIR() {
    return this.db.list<ComandoIR>(DBConstants.comandosIR, ref => ref.orderByChild('marca')).snapshotChanges().pipe(
        map(changes => changes.map(c => {
            return { key: c.key, ...c.payload.val() }
        }))
    );
  }

  updateComandoIR(comandoKey: string, newComando: ComandoIR) {
      return this.db.object<ComandoIR>(DBConstants.comandosIR + comandoKey).update(newComando);
  }

}
