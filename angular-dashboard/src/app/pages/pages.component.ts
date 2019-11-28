import { Component, ChangeDetectionStrategy, OnDestroy, OnInit } from '@angular/core';

import { MENU_ITEMS } from './pages-menu';
import { FSMenuItem } from '../models/fsMenuItem';
import { FiredbService } from '../services/firedb.service';
import { NbMenuService, NbMenuItem } from '@nebular/theme';
import { icon } from 'leaflet';
import { takeWhile } from 'rxjs/operators';
import { AulaService } from '../services/aula.service';
import { isNgTemplate } from '@angular/compiler';
import { Aula } from '../models/aula';

@Component({
  selector: 'ngx-pages',
  styleUrls: ['pages.component.scss'],
  template: `
    <ngx-one-column-layout>
      <nb-menu [items]="menu" tag="sidebarMenu" (click)="getItem()"></nb-menu>
      <router-outlet></router-outlet>
    </ngx-one-column-layout>
  `,
})
export class PagesComponent implements OnInit, OnDestroy {

  menu = MENU_ITEMS;
  private alive: boolean = true;

  constructor(
    public firedbService: FiredbService,
    public nbMenuService: NbMenuService,
    public aulaService: AulaService
  ) { }
  
  ngOnInit() {
    this.firedbService.getAulas().subscribe(edificios => {
      this.inicializarMenu(edificios);
      this.getItem();
    });

  }

  ngOnDestroy() {
    this.alive = false;
  }

  public inicializarMenu(edificios: any) {
    let aulas: FSMenuItem[];
    let departamentos: FSMenuItem[];
    departamentos = [];
    let m2: FSMenuItem;
    for (const deptoKey in edificios) {
      if (edificios.hasOwnProperty(deptoKey)) {
        aulas = [];
        m2 = new FSMenuItem();
        let depto = edificios[deptoKey];
        for (const aulaKey in depto) {
          if (depto.hasOwnProperty(aulaKey)) {
            const aula = depto[aulaKey];
            let m = new FSMenuItem();
            m.title = aulaKey;
            m.aulaNombre = aulaKey;
            m.departamento = deptoKey;
            m.nodoMac = aula.NodoMAC;
            m.link = ('/pages/'+deptoKey+'/'+aulaKey).toLocaleLowerCase();
            aulas.push(m);
          }
        }
        m2.title = deptoKey;
        m2.icon = 'keypad-outline';
        m2.children = aulas;
        departamentos.push(m2);
      }
    }
    this.nbMenuService.addItems(departamentos, 'sidebarMenu');
  }

  public getItem() {
    const subs = this.nbMenuService.getSelectedItem()
      .pipe(takeWhile(() => this.alive))
      .subscribe((nbMenuBag) => {
        const fsMenuItem = new FSMenuItem(nbMenuBag.item);
        const currentAula = this.aulaService.currentAula;
        if(currentAula.nodoMac !== fsMenuItem.nodoMac) {
          const newAula = new Aula(fsMenuItem.departamento,fsMenuItem.aulaNombre,fsMenuItem.nodoMac);
          this.aulaService.updateAula(newAula);
        }
      });
  }
}
