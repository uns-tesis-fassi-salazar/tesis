import { Injectable } from '@angular/core';
import { Observable, BehaviorSubject, Subject } from 'rxjs';
import { Aula } from '../models/aula';
import { NbMenuService } from '@nebular/theme';
import { FSMenuItem } from '../models/fsMenuItem';
import { AngularFireDatabase, SnapshotAction } from '@angular/fire/database';
import { map } from 'rxjs/operators';
import { Edificio, Nodo } from '../models';
import { groupBy } from '../utils';
import { NodoService } from './nodo.service';
import { DBConstants } from '../helpers';

@Injectable({
    providedIn: 'root'
})
export class AulaService {

    public aulaSource: BehaviorSubject<Aula>;
    public aulaData: Observable<Aula>;
    public edificioSource: BehaviorSubject<Edificio>;
    public edificioData: Observable<Edificio>;

    constructor(private db: AngularFireDatabase, private nodoSercive: NodoService) {
        this.aulaSource = new BehaviorSubject<Aula>(new Aula());
        this.aulaData = this.aulaSource.asObservable();
        this.edificioSource = new BehaviorSubject<Edificio>(new Edificio());
        this.edificioData = this.edificioSource.asObservable();
    }

    get currentAula() {
        return this.aulaSource.value;
    }

    updateCurrentAula(newAula: Aula) {
        this.aulaSource.next(newAula);
    }

    get currentEdificio() {
        return this.edificioSource.value;
    }

    updateCurrentEdificio(newEdificio: Edificio) {
        this.edificioSource.next(newEdificio);
    }

    getAulas() {
        return this.db.list<Aula>('aulas', ref => ref.orderByChild('nombre')).snapshotChanges().pipe(
            map(changes => changes.map(c => {
                return { key: c.key, ...c.payload.val() }
            }))
        );
    }

    getAulaByKey(key: string) {
        return this.db.object<Aula>('aulas/' + key).snapshotChanges()
            .pipe(map(c => {
                return { key: c.key, ...c.payload.val() }
            }));
    }

    getAulasByEdificio(edificio: Edificio) {
        return this.db.list<Aula>('aulas', ref => ref.orderByChild('edificio').equalTo(edificio.nombre)).snapshotChanges().pipe(
            map(changes => changes.map(c => {
                return { key: c.key, ...c.payload.val() };
            }))
        );
    }

    getAulasGroupByEdificio() {
        return this.db.list<Aula>('aulas', ref => ref.orderByChild('edificio')).snapshotChanges()
            .pipe(
                map(changes => changes.map(c => {
                    return { key: c.key, ...c.payload.val() }
                })),
                map<Aula[], Aula[][]>(aulas => groupBy(aulas, aula => aula.edificio))
            );
    }

    async addAula(aula: Aula) {
        if (aula.nodoMac == '') {
            return this.db.list<Aula>('aulas').push(aula);
        }
        // Actualizar las aulas que usan el nodo asignado
        const ref = this.db.database.ref(DBConstants.aulas);
        await ref.orderByChild('nodoMac').equalTo(aula.nodoMac).once('value', snapshot => {
            const aulasSinNodoMac = {};
            snapshot.forEach(snapshotChild => {
                aulasSinNodoMac[snapshotChild.key] = snapshotChild.val();
                aulasSinNodoMac[snapshotChild.key].nodoMac = '';
            });
            ref.update(aulasSinNodoMac);
        });
        const result = await this.db.list<Aula>('aulas').push(aula);
        return this.nodoSercive.updateNodoAula(aula.nodoMac, result.key);
    }

    async updateAula(oldAula: Aula, newAula: Aula) {
        // actualizar el aula del nodo viejo y nuevo
        if (oldAula.nodoMac != '')
            await this.nodoSercive.updateNodoAula(oldAula.nodoMac, '');
        if (newAula.nodoMac != '') {
            await this.nodoSercive.updateNodoAula(newAula.nodoMac, oldAula.key);
            // Actualizar las aulas que usaban el nuevo nodo
            const ref = this.db.database.ref('aulas');
            await ref.orderByChild('nodoMac').equalTo(newAula.nodoMac).once('value', snapshot => {
                const aulasSinNodoMac = {};
                snapshot.forEach(snapshotChild => {
                    aulasSinNodoMac[snapshotChild.key] = snapshotChild.val();
                    aulasSinNodoMac[snapshotChild.key].nodoMac = '';
                });
                ref.update(aulasSinNodoMac);
            });
        }
        return this.db.list<Aula>(DBConstants.aulas).update(oldAula.key, newAula);
    }

    async removeAula(aula: Aula) {
        if (aula.nodoMac != '') {
            await this.nodoSercive.updateNodoAula(aula.nodoMac, '');
        }
        return this.db.list<Aula>('aulas').remove(aula.key);
    }

    // Edificios

    getEdificios() {
        return this.db.list<Edificio>('edificios', ref => ref.orderByChild('nombre')).snapshotChanges().pipe(
            map(changes => changes.map(c => {
                return { key: c.key, ...c.payload.val() }
            }))
        );
    }

    addEdificio(edificio: Edificio) {
        return this.db.list<Edificio>('edificios').push(edificio);
    }

    async updateEdificio(oldEdificio: Edificio, newEdificio: Edificio) {
        // Actualizar las aulas del edificio viejo con el nuevo
        const ref = this.db.database.ref(DBConstants.aulas);
        await ref.orderByChild(DBConstants.aulaEdificio).equalTo(oldEdificio.nombre).once('value', snapshot => {
            const aux = {};
            snapshot.forEach(snapshotChild => {
                aux[snapshotChild.key] = snapshotChild.val();
                aux[snapshotChild.key].edificio = newEdificio.nombre;
            });
            ref.update(aux);
        });
        return this.db.list<Edificio>(DBConstants.edificios).update(oldEdificio.key, newEdificio);
    }

    removeEdificio(edificio: Edificio) {
        return this.db.list<Edificio>('edificios').remove(edificio.key);
    }

}
