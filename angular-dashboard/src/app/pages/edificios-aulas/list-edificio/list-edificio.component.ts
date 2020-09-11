import { Component, OnInit, OnDestroy } from '@angular/core';
import { Observable } from 'rxjs';
import { Edificio } from '../../../models';
import { AulaService, UtilService } from '../../../services';
import { Router, ActivatedRoute } from '@angular/router';
import { NbDialogService } from '@nebular/theme';
import { takeWhile } from 'rxjs/operators';
import { DecisionDialogComponent } from '../../../utils';
import { UrlRoutes } from '../../../helpers';

@Component({
  selector: 'list-edificio',
  templateUrl: './list-edificio.component.html',
  styleUrls: ['./list-edificio.component.scss']
})
export class ListEdificioComponent implements OnInit, OnDestroy {

  alive = true;
  optionsClicked = false;
  edificios$: Observable<Edificio[]>;

  constructor(public aulaService: AulaService,
    private router: Router,
    private route: ActivatedRoute,
    private dialogService: NbDialogService,
    private utilService: UtilService,
  ) { }

  ngOnInit() {
    this.edificios$ = this.aulaService.getEdificios()
      .pipe(takeWhile(() => this.alive));
  }

  ngOnDestroy() {
    this.alive = false;
  }

  onClickEdificio(edificio: Edificio) {
    this.aulaService.updateCurrentEdificio(edificio);
    this.router.navigate([UrlRoutes.aulasEdificio], { relativeTo: this.route });
  }

  onClickNuevoEdificio() {
    this.router.navigate([UrlRoutes.nuevoEdificio], { relativeTo: this.route });
  }

  onClickNuevaAula() {
    this.router.navigate([UrlRoutes.nuevaAula], { relativeTo: this.route });
  }

  editarEdificio(edificio: Edificio) {
    this.aulaService.updateCurrentEdificio(edificio);
    this.router.navigate([UrlRoutes.edificios, UrlRoutes.editarEdificio]);
  }

  async eliminarEdificio(edificio: Edificio) {
    this.aulaService.getAulasByEdificio(edificio)
      .pipe(takeWhile(() => this.alive))
      .subscribe(aulas => {
        if (aulas.length == 0) {
          this.dialogService.open(DecisionDialogComponent, {
            context: {
              message: 'Se eliminara el departamento seleccionado.'
            }
          }).onClose
          .pipe(takeWhile(() => this.alive))
          .subscribe(respuesta => {
            if (respuesta) {
              this.aulaService.removeEdificio(edificio)
                .then(res => {
                  this.utilService.showToast('success', 'Edificio eliminado satisfactoriamente!');
                })
                .catch(err => this.utilService.showError(err, 'Error al eliminar el edificio', 'Es posible que se deba a un fallo en la comunicación', 4000))
            } else {
              this.optionsClicked = false;
            }
          });
        } else {
          this.utilService.showError(err, 'No se puede eliminar el edificio', 'Primero elimine las aulas que contiene', 4000)
        }
    });
  }
}
