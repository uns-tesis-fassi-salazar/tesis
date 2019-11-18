import { Injectable } from '@angular/core';
import { AngularFireDatabase } from '@angular/fire/database';

@Injectable({
  providedIn: 'root'
})
export class FiredbService {

  constructor(
    private db: AngularFireDatabase
  ) { }

  getNodoMAC(){
    return this.db.object('Edificios/DCIC/Labo4/NodoMAC').valueChanges();
  }

  getLuminocidad(NodoMAC){
    return this.db.object('Nodos/' + NodoMAC + '/Sensores/Luminocidad').valueChanges();
  }

  getHumedad(NodoMAC){
    return this.db.object('Nodos/' + NodoMAC + '/Sensores/Humedad').valueChanges();
  }

  getTemperatura(NodoMAC){
    return this.db.object('Nodos/' + NodoMAC + '/Sensores/Temperatura').valueChanges();
  }

  getMovimiento(NodoMAC){
    return this.db.object('Nodos/' + NodoMAC + '/Sensores/Movimiento').valueChanges();
  }

  updateActuador(nodoId, actuadorId, value){
    this.db.object(nodoId + '/Actuadores/' + actuadorId).update({Estado:value});
  }

}
