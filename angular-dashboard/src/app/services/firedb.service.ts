import { Injectable } from '@angular/core';
import { AngularFireDatabase } from '@angular/fire/database';

@Injectable({
  providedIn: 'root'
})
export class FiredbService {

  constructor(private db: AngularFireDatabase) { }

  getTest(){
    return this.db.object('ESP32_Test/Double').valueChanges();
  }

  getSensor(nodoId, sensorId){
    return this.db.object(nodoId + '/Sensores/' + sensorId).valueChanges();
  }

  getActuador(nodoId, actuadorId){
    return this.db.object(nodoId + '/Actuadores/' + actuadorId).valueChanges();

  }

  updateActuador(nodoId, actuadorId, value){
    this.db.object(nodoId + '/Actuadores/' + actuadorId).update({Estado:value});
  }

}
