import { Injectable } from '@angular/core';
import { AngularFireDatabase, AngularFireList } from '@angular/fire/database';
import { Observable } from 'rxjs';
import { AngularFireStorage } from '@angular/fire/storage';
import { Nodo } from '../models';
import { map } from 'rxjs/operators';
import { DBConstants } from '../helpers';

@Injectable({
  providedIn: 'root'
})
export class NodoService {

  constructor(
    private db: AngularFireDatabase,
    private storage: AngularFireStorage,
  ) { }

  getNodos() {
    return this.db.list<Nodo>(DBConstants.nodos).snapshotChanges()
      .pipe(
        map(changes => changes.map(c => {
          return { mac: c.key, ...c.payload.val() }
        }))
      )
  }

  getNodoByMac(mac: string) {
    return this.db.object<Nodo>(DBConstants.nodos + mac).valueChanges();
  }

  getSensor(nodoMac, sensorId) {
    return this.db.object(DBConstants.nodos + nodoMac + '/' + DBConstants.nodoSensores + sensorId).valueChanges();
  }

  updateNodo(oldNodo: Nodo, newNodo: Nodo) {
    return this.db.list<Nodo>(DBConstants.nodos).update(oldNodo.mac, newNodo);
  }

  async updateNodoAula(mac: string, newAulaKey: string) {
    await this.db.object(DBConstants.nodos + mac).update({
      [DBConstants.nodoAulaAsignada]: newAulaKey,
      [DBConstants.nodoComando]: DBConstants.comandoActualizarAula
    });
    return this.resetNodoComando(mac);
  }

  updateNodoComando(nodoMac, comandoValue) {
    this.db.object(DBConstants.nodos + nodoMac).update({ [DBConstants.nodoComando]: comandoValue });
    this.resetNodoComando(nodoMac);
  }

  uploadFileTask(nombreArchivo: string, datos: any) {
    return this.storage.upload(nombreArchivo, datos);
  }

  //Referencia del archivo
  getFileReference(nombreArchivo: string) {
    return this.storage.ref(nombreArchivo);
  }

  // Guardar en la db la información del firmware subido
  saveFirmwareMetadata(filename, version, url) {
    const metadata = {
      dateUploaded: Date.now(),
      filename: filename,
      url: url
    }

    return this.db.object('firmware/' + version).set(metadata);
  }

  // Actualizar la version del firmware en todos los nodos
  async updateNodosFirmwareVersion(version: string) {
    const ref = this.db.database.ref(DBConstants.nodos);
    return ref.once('value', async snapshot => {
      const nodosConNuevaVersion = {};
      snapshot.forEach(snapshotChild => {
        nodosConNuevaVersion[snapshotChild.key] = snapshotChild.val();
        nodosConNuevaVersion[snapshotChild.key][DBConstants.nodoComando] = DBConstants.comandoActualizarFirmware;
        nodosConNuevaVersion[snapshotChild.key][DBConstants.nodoFirmwareVersion] = version;
      });
      await ref.update(nodosConNuevaVersion);
      this.resetAllNodosComando();
    });
  }

  resetAllNodosComando() {
    setTimeout(() => {
      const ref = this.db.database.ref(DBConstants.nodos);
      ref.once('value', snapshot => {
        const nodosConNuevaVersion = {};
        snapshot.forEach(snapshotChild => {
          nodosConNuevaVersion[snapshotChild.key] = snapshotChild.val();
          nodosConNuevaVersion[snapshotChild.key][DBConstants.nodoComando] = DBConstants.comandoReset;
        });
        ref.update(nodosConNuevaVersion);
      });
    }, 1500);
  }

  resetNodoComando(mac: string) {
    setTimeout(() => {
      this.db.object(DBConstants.nodos + mac).update({ [DBConstants.nodoComando]: 0 });
    }, 1500);
  }

}
