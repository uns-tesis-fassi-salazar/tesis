import { Component, OnInit, OnDestroy } from '@angular/core';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { AulaService, UtilService } from '../../../services';
import { Router } from '@angular/router';
import { UrlRoutes } from '../../../helpers';
import { takeWhile } from 'rxjs/operators';
import { NbDialogService } from '@nebular/theme';
import { DecisionDialogComponent } from '../../../utils';

@Component({
  selector: 'edit-edificio',
  templateUrl: './edit-edificio.component.html',
  styleUrls: ['./edit-edificio.component.scss']
})
export class EditEdificioComponent implements OnInit, OnDestroy {

  registerForm: FormGroup;
  submitted = false;
  alive = true;

  constructor(
    private formBuilder: FormBuilder,
    private aulaService: AulaService,
    private utilService: UtilService,
    private router: Router,
    private dialogService: NbDialogService) { }

  ngOnInit() {
    if (!this.aulaService.currentEdificio || this.aulaService.currentEdificio.key == null)
      this.navigateToEdificiosList();

    this.registerForm = this.formBuilder.group({
      nombre: ['', Validators.required],
    });

    this.registerForm.patchValue(this.aulaService.currentEdificio);
  }

  ngOnDestroy() {
    this.alive = false;
  }

  get f() { return this.registerForm.controls; }

  onSubmit() {
    this.submitted = true;

    if (this.registerForm.invalid) {
      return;
    }

    this.aulaService.updateEdificio(this.aulaService.currentEdificio, this.registerForm.value)
      .then(res => {
        this.utilService.showToast('success', 'Edificio editado satisfactoriamente!');
        this.navigateToEdificiosList();
      })
      .catch(err => this.utilService.showToast('warning', 'Error al editar el edificio', 'Es posible que se deba a un fallo en la comunicación', 4000))
  }

  // TODO: eliminar las aulas que contenga el edificio
  eliminarEdificio() {
    this.aulaService.getAulasByEdificio(this.aulaService.currentEdificio)
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
                this.aulaService.removeEdificio(this.aulaService.currentEdificio)
                  .then(res => {
                    this.utilService.showToast('success', 'Edificio eliminado satisfactoriamente!');
                    this.navigateToEdificiosList();
                  })
                  .catch(err => this.utilService.showToast('warning', 'Error al eliminar el edificio', 'Es posible que se deba a un fallo en la comunicación', 4000))
              }
            });
        } else {
          this.utilService.showToast('warning', 'No se puede eliminar el edificio', 'Primero elimine las aulas que contiene', 4000)
        }
      });
  }

  navigateToEdificiosList() {
    this.router.navigate([UrlRoutes.edificios])
  }

  resetForm() {
    this.submitted = false;
    this.registerForm.reset();
  }
}
