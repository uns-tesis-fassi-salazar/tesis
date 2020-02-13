import { Component, OnInit, OnDestroy } from '@angular/core';
import { Observable } from 'rxjs';
import { Aula, Nodo } from '../../models';
import { AulaService, NodoService } from '../../services';
import { takeWhile, take } from 'rxjs/operators';
import { Router } from '@angular/router';
import { UrlRoutes } from '../../helpers';

@Component({
  selector: 'logs',
  templateUrl: './logs.component.html',
  styleUrls: ['./logs.component.scss']
})
export class LogsComponent implements OnInit, OnDestroy {
  nodos: Nodo[];
  alive = true;

  constructor(public aulaService: AulaService,
    public nodoService: NodoService,
    private router: Router) { }

  async ngOnInit() {
    this.nodos = await this.getNodosYAulas();
  }

  ngOnDestroy() {
    this.alive = false;
  }

  async getNodosYAulas() {
    const nodos = await this.nodoService.getNodos().pipe(take(1)).toPromise();
    const aulas = await this.aulaService.getAulas().pipe(take(1)).toPromise();
    return nodos.map((nodo) => {
      const aulaDelNodo = aulas.filter((aula) => aula.nodoMac == nodo.mac);
      let aula;
      if (aulaDelNodo.length == 0)
        aula = 'Sin aula';
      else
        aula = aulaDelNodo[0].edificio + ' - ' + aulaDelNodo[0].nombre;
      const nuevoNodo = new Nodo()
      nuevoNodo.mac = nodo.mac;
      nuevoNodo.aulaAsignada = aula;
      return nuevoNodo;
    })
  }

  onClickNodo(nodo: Nodo) {
    this.router.navigate([UrlRoutes.logs, nodo.mac]);
  }

}
