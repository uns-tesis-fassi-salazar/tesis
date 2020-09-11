import { Component, OnInit } from '@angular/core';
import { FormGroup, FormBuilder, Validators } from '@angular/forms';
import { Observable } from 'rxjs';
import { Edificio } from '../../../models';
import { AulaService, UtilService } from '../../../services';
import { NbToastrService } from '@nebular/theme';
import { Router } from '@angular/router';
import { UrlRoutes } from '../../../helpers';

@Component({
  selector: 'create-edificio',
  templateUrl: './create-edificio.component.html',
  styleUrls: ['./create-edificio.component.scss']
})
export class CreateEdificioComponent implements OnInit {

  registerForm: FormGroup;
  submitted = false;

  constructor(
    private formBuilder: FormBuilder,
    private aulaService: AulaService,
    private utilService: UtilService,
    private router: Router) { }

  ngOnInit() {
    this.registerForm = this.formBuilder.group({
      nombre: ['', Validators.required],
    });

  }

  get f() { return this.registerForm.controls; }

  onSubmit() {
    this.submitted = true;

    if (this.registerForm.invalid) {
      return;
    }

    // TODO: controlar que no se guarden edificios repetidos
    this.aulaService.addEdificio(this.registerForm.value)
      .then( res => {
        this.utilService.showToast('success','Edificio creado satisfactoriamente!');
        this.navigateToEdificios();
      })
      .catch(err => {
        this.utilService.showError(err, 'Error al guardar el edificio', 'Es posible que se deba a un fallo en la comunicación');
      })
  }

  navigateToEdificios() {
    this.router.navigate([UrlRoutes.edificios]);
  }

  resetForm() {
    this.submitted = false;
    this.registerForm.reset();
  }

  

}
