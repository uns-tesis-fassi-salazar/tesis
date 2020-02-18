import { Component, OnInit, OnDestroy } from '@angular/core';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { Observable } from 'rxjs';
import { Edificio, Aula, Nodo, ComandoIR } from '../../../models';
import { NbDialogService } from '@nebular/theme';
import { AulaService, UtilService, NodoService, IRService } from '../../../services';
import { Router } from '@angular/router';
import { takeWhile, take } from 'rxjs/operators';
import { DecisionDialogComponent } from '../../../utils';
import { UrlRoutes } from '../../../helpers';

@Component({
  selector: 'edit-aula',
  templateUrl: './edit-aula.component.html',
  styleUrls: ['./edit-aula.component.scss']
})
export class EditAulaComponent implements OnInit, OnDestroy {
  alive = true;
  registerForm: FormGroup;
  submitted = false;
  edificios$: Observable<Edificio[]>;
  nodos$: Observable<Nodo[]>;
  comandosIR$: Observable<ComandoIR[]>;

  constructor(
    private formBuilder: FormBuilder,
    public aulaService: AulaService,
    private utilService: UtilService,
    private nodoService: NodoService,
    private router: Router,
    private dialogService: NbDialogService,
    private irService: IRService) { }

  ngOnInit() {
    if (!this.aulaService.currentAula || this.aulaService.currentAula.key == null)
      this.navigateToAulasList();

    this.registerForm = this.formBuilder.group({
      nombre: ['', Validators.required],
      edificio: ['', Validators.required],
      nodoMac: [''],
      comandoIR: [''],
      timeoutAulaVacia: [1, Validators.required],
      intervaloLecturas: [5, Validators.required],
      horaInicioAuto: ['22:00', Validators.required],
      horaFinAuto: ['06:00', Validators.required],
      // umbralSensorHall: [2.5],
      // zeroSensorHall: [1810],
    });

    this.nodos$ = this.nodoService.getNodos()
      .pipe(takeWhile(() => this.alive));
    
    this.edificios$ = this.aulaService.getEdificios()
      .pipe(takeWhile(() => this.alive));

    this.comandosIR$ = this.irService.getComandosIR()
      .pipe(takeWhile(() => this.alive));

    this.edificios$.pipe(take(1)).subscribe(x => {
      setTimeout(() => {
        // TODO: Solucionar este parche. No encontre otra forma de 
        // actualizar el valor del select en la vista
        this.registerForm.patchValue(this.aulaService.currentAula);
      }, 100);
    });

    this.nodos$.pipe(take(1)).subscribe(x => {
      setTimeout(() => {
        // TODO: Solucionar este parche. No encontre otra forma de 
        // actualizar el valor del select en la vista
        this.registerForm.patchValue(this.aulaService.currentAula);
      }, 100);
    });

    this.comandosIR$.pipe(take(1)).subscribe(x => {
      setTimeout(() => {
        // TODO: Solucionar este parche. No encontre otra forma de 
        // actualizar el valor del select en la vista
        this.registerForm.patchValue(this.aulaService.currentAula);
      }, 100);
    });
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

    this.aulaService.updateAula(this.aulaService.currentAula, this.registerForm.value)
      .then(res => {
        this.utilService.showToast('success', 'Aula editada satisfactoriamente!');
        this.navigateToAulasList();
      })
      .catch(err => this.utilService.showToast('warning', 'Error al editar el aula', 'Es posible que se deba a un fallo en la comunicación', 4000))
  }

  async eliminarAula() {
    this.dialogService.open(DecisionDialogComponent, { 
      context: { 
        message: 'Se eliminara el aula seleccionada'
      } 
    }).onClose
    .pipe(takeWhile(() => this.alive))
    .subscribe( respuesta => {
      if (respuesta) {
        this.aulaService.removeAula(this.aulaService.currentAula)
        .then(res => {
          this.utilService.showToast('success', 'Aula eliminada satisfactoriamente!');
          this.navigateToAulasList();
        })
        .catch(err => this.utilService.showToast('warning', 'Error al eliminar el aula', 'Es posible que se deba a un fallo en la comunicación', 4000))
      }
    });
  }

  navigateToAulasList() {
    this.router.navigate([UrlRoutes.edificios,UrlRoutes.aulasEdificio]);
  }

  resetForm() {
    this.submitted = false;
    this.registerForm.reset();
  }

}
