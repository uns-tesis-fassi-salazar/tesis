import { Component, OnInit, OnDestroy } from '@angular/core';
import { Observable, pipe } from 'rxjs';
import { Aula, Edificio } from '../../../models';
import { AulaService, UtilService } from '../../../services';
import { Router, ActivatedRoute } from '@angular/router';
import { takeWhile } from 'rxjs/operators';
import { NbDialogService } from '@nebular/theme';
import { DecisionDialogComponent } from '../../../utils';

@Component({
  selector: 'configuration-aula',
  templateUrl: './configuration-aula.component.html',
  styleUrls: ['./configuration-aula.component.scss']
})
export class ConfigurationAulaComponent implements OnInit, OnDestroy {

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
    this.router.navigate(['/edificios/aulas'], { relativeTo: this.route });
  }

  editarEdificio(edificio: Edificio) {
    this.aulaService.updateCurrentEdificio(edificio);
    this.router.navigate(['/edificios/editar-edificio']);
  }

  eliminarEdificio(edificio: Edificio) {
    this.dialogService.open(DecisionDialogComponent, {
      context: {
        message: 'Se perderan todas las aulas que contenga.'
      }
    }).onClose.subscribe(respuesta => {
      if (respuesta) {
        this.aulaService.removeEdificio(edificio)
          .then(res => {
          })
          .catch(err => this.utilService.showToast('warning', 'Error al eliminar el edificio', 'Es posible que se deba a un fallo en la comunicación', 4000))
      } else {
        this.optionsClicked = false;
      }
    });
  }

}
