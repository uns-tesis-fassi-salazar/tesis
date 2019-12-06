import { Injectable } from '@angular/core';
import { AngularFireDatabase } from '@angular/fire/database';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class FiredbService {

  constructor(
    private db: AngularFireDatabase
  ) { }

  getnodoMac(){
    return this.db.object('Edificios/DCIC/Labo4/nodoMac').valueChanges();
  }

  getConfig(nodoMac, configId) {
    return this.db.object('Nodos/' + nodoMac + '/Configuracion/' + configId).valueChanges();
  }

  getSensor(nodoMac, sensorId) {
    return this.db.object('Nodos/' + nodoMac + '/Sensores/' + sensorId).valueChanges();
  }

  getActuador(nodoMac, actuadorId){
    return this.db.object('Nodos/' + nodoMac +'/Actuadores/' + actuadorId).valueChanges();
  }

  updateActuador(nodoMac, actuadorId, value){
    this.db.object('Nodos/' + nodoMac + '/Actuadores').update({[actuadorId]:value ? 1 : 0});
  }

  getAulas() {
    return this.db.object('Edificios/').valueChanges();
  }

}
