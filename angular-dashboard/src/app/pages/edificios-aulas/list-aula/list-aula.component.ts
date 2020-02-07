import { Component, OnInit, OnDestroy } from '@angular/core';
import { Observable } from 'rxjs';
import { Aula, Edificio } from '../../../models';
import { AulaService, UtilService } from '../../../services';
import { Router } from '@angular/router';
import { takeWhile } from 'rxjs/operators';
import { NbDialogService } from '@nebular/theme';
import { DecisionDialogComponent } from '../../../utils';
import { UrlRoutes } from '../../../helpers';

@Component({
  selector: 'list-aula',
  templateUrl: './list-aula.component.html',
  styleUrls: ['./list-aula.component.scss']
})
export class ListAulaComponent implements OnInit, OnDestroy {
  alive = true;
  aulas$: Observable<Aula[]>;
  optionsClicked = false;

  constructor(public aulaService: AulaService,
    private router: Router,
    private utilService: UtilService,
    private dialogService: NbDialogService) { }

  ngOnInit() {
    if (!this.aulaService.currentEdificio || this.aulaService.currentEdificio.key == null)
      this.volver();

    this.aulas$ = this.aulaService.getAulasByEdificio(this.aulaService.currentEdificio)
      .pipe(takeWhile(() => this.alive));
  }

  ngOnDestroy() {
    this.alive = false;
  }
  onClickAula(aula: Aula) {
    this.router.navigate([UrlRoutes.dashboardAula,aula.key]);
  }

  volver() {
    this.router.navigate([UrlRoutes.edificios]);
  }

  editarAula(aula: Aula) {
    this.aulaService.updateCurrentAula(aula);
    this.router.navigate([UrlRoutes.edificios, UrlRoutes.aulasEdificio, UrlRoutes.editarAula]);
  }

  eliminarAula(aula: Aula) {
    this.dialogService.open(DecisionDialogComponent, {
      context: {
        message: 'Se eliminara el aula seleccionada'
      }
    }).onClose.subscribe(respuesta => {
      if (respuesta) {
        this.aulaService.removeAula(aula)
          .then(res => {
          })
          .catch(err => this.utilService.showToast('warning', 'Error al eliminar el aula', 'Es posible que se deba a un fallo en la comunicación', 4000))
      }
      this.optionsClicked = false;
    });
  }

}
