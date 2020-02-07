import { Component, OnInit } from '@angular/core';
import { NodoService } from '../../../services';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { finalize } from 'rxjs/operators';
import { Observable } from 'rxjs';

@Component({
  selector: 'configuration-firmware',
  templateUrl: './configuration-firmware.component.html',
  styleUrls: ['./configuration-firmware.component.scss']
})
export class ConfigurationFirmwareComponent implements OnInit {
  registerForm: FormGroup;
  file: any;
  nombreArchivo = '';
  URLPublica$: Observable<string>;
  porcentajeSubida: number = 0;
  finalizado = false;
  submitted = false;
  otherError = false;
  errorMessage = '';

  constructor(private formBuilder: FormBuilder,
    private nodoService: NodoService) { }

  ngOnInit() {
    this.registerForm = this.formBuilder.group({
      archivo: [null, Validators.required],
    });

  }

  //Evento que se gatilla cuando el input de tipo archivo cambia
  cambioArchivo(event) {
    this.resetForm();

    if (event.target.files.length == 1) {
      this.nombreArchivo = event.target.files[0].name;
      this.file = event.target.files[0];
    } else {
      this.nombreArchivo = '';
    }
  }

  get f() { return this.registerForm.controls; }

  onSubmit() {
    this.submitted = true;

    if (this.registerForm.invalid) {
      return;
    }

    this.readFile(this.file);
  }

  // Read the file chosen by the user.
  private readFile(file) {
    // Open the file and start reading it.
    var reader = new FileReader();
    reader.onloadend = () => {
      this.readMetadata(file, reader.result);
    }
    reader.readAsArrayBuffer(file);
  }

  // Extract the version string from the given file data.
  private getFirmwareVersion(data) {
    var enc = new TextDecoder("utf-8");
    var s = enc.decode(data);
    var re = new RegExp(/__AuLaSSuStEnTaBlEs__ [^_]+__/);
    var result = re.exec(s);
     if (result == null) {
      return null;
    }
    return result[0];
  }

  // Called when we're done reading the file data.
  private readMetadata(file, data) {
    const version = this.getFirmwareVersion(data);
    if (version == null) {
      this.otherError = true;
      this.errorMessage = 'No se pudo extraer la version del binario.';
      return;
    }

    // Upload firmware binary to Firebase Cloud Storage.
    // We use the version string as the filename, since
    // it's assumed to be unique.
    this.uploadFileToFirebase(version);

  }

  uploadFileToFirebase(version: string) {
    let referencia = this.nodoService.getFileReference(version);
    let tarea = this.nodoService.uploadFileTask(version, this.file);

    tarea.percentageChanges().subscribe(procentaje => {
      this.porcentajeSubida = procentaje;
    });

    // get notified when the download URL is available
    tarea.snapshotChanges().pipe(
      finalize(() => {
        this.URLPublica$ = referencia.getDownloadURL();
        this.URLPublica$.subscribe( imageUrl => {
          this.nodoService.saveFirmwareMetadata(this.file.name, version, imageUrl)
            .then( exito => this.nodoService.updateNodosFirmwareVersion(version)
              .then( x => console.log('Base de datos actualizada!'))
              .catch( e => console.log('Error al actualizar la version de cada nodo'))
            )
            .catch( error => console.log('Error al guardar firmware metadata en la db',error));
        });
        this.finalizado = true;
      })
    ).subscribe();
  }

  // Subir archivo
  // Actualizar base de datos con la información de la nueva version

  resetForm() {
    this.submitted = false;
    this.finalizado = false;
    this.porcentajeSubida = 0
    this.otherError = false;
  }
}
