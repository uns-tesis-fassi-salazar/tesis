import { Component, ChangeDetectionStrategy, OnDestroy, OnInit } from '@angular/core';

import { MENU_ITEMS } from './pages-menu';
import { FSMenuItem } from '../models/fsMenuItem';
import { NodoService } from '../services/nodo.service';
import { NbMenuService, NbMenuItem, NbSidebarService } from '@nebular/theme';
import { icon } from 'leaflet';
import { takeWhile } from 'rxjs/operators';
import { AulaService } from '../services/aula.service';
import { isNgTemplate } from '@angular/compiler';
import { Aula } from '../models/aula';
import { Subscription } from 'rxjs';

@Component({
  selector: 'ngx-pages',
  styleUrls: ['pages.component.scss'],
  template: `
    <ngx-one-column-layout>
      <nb-menu [items]="menu" [tag]="menuTag"></nb-menu>
      <router-outlet></router-outlet>
    </ngx-one-column-layout>
    `,
})
export class PagesComponent implements OnInit, OnDestroy {
  private alive: boolean = true;
  menu = MENU_ITEMS.map((value) => value); // clone del menu
  menuTag = 'sidebarMenu';

  constructor(
    public firedbService: NodoService,
    public nbMenuService: NbMenuService,
    public aulaService: AulaService
  ) { }

  ngOnInit() {
    this.aulaService.getAulasGroupByEdificio()
      .pipe(takeWhile(() => this.alive))
      .subscribe(aulasGroupByEdificio => {
        this.inicializarMenu(aulasGroupByEdificio);
      });
  }

  ngOnDestroy() {
    this.alive = false;
  }
  
  public inicializarMenu(aulasGroupByEdificio: Aula[][]) {
    const menuAux = MENU_ITEMS.map((v) => v); // clone
    for (const edificio in aulasGroupByEdificio) {
      if (aulasGroupByEdificio.hasOwnProperty(edificio)) {
        let nuevoEdificioItem = new NbMenuItem();
        nuevoEdificioItem.title = edificio;
        // nuevoEdificioItem.icon = 'cube-outline';
        nuevoEdificioItem.children = [];
        
        const aulas = aulasGroupByEdificio[edificio];
        for (const aula of aulas) {
          let nuevoAulaItem = new NbMenuItem();
          nuevoAulaItem.title = aula.nombre;
          nuevoAulaItem.link = '/dashboard/aula/' + aula.key;

          nuevoEdificioItem.children.push(nuevoAulaItem);
        }
        menuAux.push(nuevoEdificioItem);
      }
    }
    this.menu = menuAux;
  }
}
