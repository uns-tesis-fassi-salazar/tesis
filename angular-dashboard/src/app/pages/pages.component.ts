import { Component } from '@angular/core';

import { MENU_ITEMS } from './pages-menu';
import { AulasService } from '../services/aulas.service';
import { FiredbService } from '../services/firedb.service';
import { NbMenuService, NbMenuItem } from '@nebular/theme';
import { icon } from 'leaflet';

@Component({
  selector: 'ngx-pages',
  styleUrls: ['pages.component.scss'],
  template: `
    <ngx-one-column-layout>
      <nb-menu [items]="menu" (click)="getItem()"></nb-menu>
      <router-outlet></router-outlet>
    </ngx-one-column-layout>
  `,
})
export class PagesComponent {

  // menu = MENU_ITEMS;
  menu = [];

  constructor(
    public firedbService: FiredbService,
    public nbMenuService: NbMenuService
  ) {
    firedbService.getAulas().subscribe(edificios => {
      console.log(edificios);
      this.inicializarMenu(edificios);
    });
    
  }

  public inicializarMenu(edificios: any) {
    let aulas;
    let departamentos = new Array<NbMenuItem>();
  
    for (const deptoKey in edificios) {
      aulas = new Array<NbMenuItem>();
      if (edificios.hasOwnProperty(deptoKey)) {
        let depto = edificios[deptoKey];
        for (const aulaKey in depto) {
          if (depto.hasOwnProperty(aulaKey)) {
            const aula = depto[aulaKey];
            let m = new NbMenuItem();
            m.title = aulaKey;
            m.link = '/page/dashboard';
            aulas.push(m);
          }
        }
        let m2 = new NbMenuItem();
        m2.title = deptoKey;
        m2.icon = 'keypad-outline';
        m2.children = aulas;
        departamentos.push(m2);
      }
    }
    this.nbMenuService.addItems(departamentos);
    this.menu.push(departamentos);
  }

  public getItem() {
    this.nbMenuService.getSelectedItem().subscribe(nbMenuBag => {
      console.log(nbMenuBag);
    });
    this.nbMenuService.onItemSelect().subscribe(nbMenuBag => {
      console.log("onItemClick ",nbMenuBag);
    });
  }


}
