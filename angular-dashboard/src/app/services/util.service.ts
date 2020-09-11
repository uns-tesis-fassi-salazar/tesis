import { Injectable } from '@angular/core';
import { AngularFireDatabase, AngularFireList } from '@angular/fire/database';
import { Observable } from 'rxjs';
import { NbToastrService, NbGlobalPosition, NbGlobalPhysicalPosition } from '@nebular/theme';
import { isNullOrUndefined } from 'util';

@Injectable({
  providedIn: 'root'
})
export class UtilService {

  constructor(
    private toastrService: NbToastrService
  ) { }

  showToast(status, title, message = '', duration = 2500, position = NbGlobalPhysicalPosition.BOTTOM_RIGHT) {
    this.toastrService.show(
      message,
      title,
      { position, status, duration });
  }

  showError(error, title: string, message: string = '', duration = 4000) {
    const firebaseError: firebase.FirebaseError = error;
    if (!isNullOrUndefined(firebaseError)) {
      if (firebaseError.code === 'PERMISSION_DENIED') {
        message = 'No se cuenta con los permisos suficientes.'
      }
    }
    
    this.showToast('warning', title, message, duration);
  }

}
