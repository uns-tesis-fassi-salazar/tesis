import { Component, OnInit, OnDestroy } from '@angular/core';
import { NbDialogService } from '@nebular/theme';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { AulaService, UtilService, NodoService, IRService } from '../../../services';
import { Aula, Edificio, Nodo, ComandoIR } from '../../../models';
import { Observable } from 'rxjs';
import { Router } from '@angular/router';
import { takeWhile } from 'rxjs/operators';
import { UrlRoutes } from '../../../helpers';

@Component({
  selector: 'create-aula',
  templateUrl: './create-aula.component.html',
  styleUrls: ['./create-aula.component.scss']
})
export class CreateAulaComponent implements OnInit, OnDestroy {
  alive = true;
  registerForm: FormGroup;
  submitted = false;
  edificios$: Observable<Edificio[]>;
  nodos$: Observable<Nodo[]>;
  comandosIR$: Observable<ComandoIR[]>;

  constructor(
    private dialogService: NbDialogService,
    private formBuilder: FormBuilder,
    private aulaService: AulaService,
    private utilService: UtilService,
    private nodoService: NodoService,
    private router: Router,
    private irService: IRService) { }

  ngOnInit() {
    this.registerForm = this.formBuilder.group({
      nombre: ['', Validators.required],
      edificio: ['', Validators.required],
      nodoMac: [''],
      comandoIR: [''],
      timeoutAulaVacia: [10, Validators.required],
      intervaloLecturas: [5, Validators.required],
      horaInicioAuto: ['22:00', Validators.required],
      horaFinAuto: ['06:00', Validators.required],
    });

    this.edificios$ = this.aulaService.getEdificios()
      .pipe(takeWhile(() => this.alive));

    this.nodos$ = this.nodoService.getNodos()
      .pipe(takeWhile(() => this.alive));

    this.comandosIR$ = this.irService.getComandosIR()
      .pipe(takeWhile(() => this.alive));
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

    this.aulaService.addAula(this.registerForm.value)
      .then(res => {
        this.utilService.showToast('success', 'Aula creada satisfactoriamente!');
        this.resetForm();
      })
      .catch(err => this.utilService.showToast('warning', 'Error al guardar el aula', 'Es posible que se deba a un fallo en la comunicación', 4000))
  }

  resetForm() {
    this.submitted = false;
    this.registerForm.reset();
  }

  onClickCancelar() {
    this.router.navigate([UrlRoutes.edificios]);
  }

  log(time) {
    console.log(time);
  }

}
