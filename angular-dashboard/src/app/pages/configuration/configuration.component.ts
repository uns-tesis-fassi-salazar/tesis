import { Component, OnInit } from '@angular/core';
import { NbDialogService } from '@nebular/theme';
import { AngularFireList } from '@angular/fire/database';
import { NodoService } from '../../services/nodo.service';
import { Observable } from 'rxjs';
import { AulaService } from '../../services/aula.service';
import { Aula } from '../../models';

@Component({
  selector: 'ngx-configuration',
  template: `
    <router-outlet></router-outlet>
  `,
  styleUrls: ['./configuration.component.scss']
})
export class ConfigurationComponent implements OnInit {

  constructor() { }

  ngOnInit() {
  }
  

}
