import { Injectable } from '@angular/core';
import { AngularFireAuth } from "@angular/fire/auth";
import { Observable } from 'rxjs';
import { Router } from '@angular/router';

@Injectable({
  providedIn: 'root'
})

export class AuthService {
  public currentUser: Observable<firebase.User>;

  constructor(private angularFireAuth: AngularFireAuth, private router: Router) {
    this.currentUser = angularFireAuth.authState;
    angularFireAuth.auth.onAuthStateChanged((user) => {
      if (user) {
        console.log('user is logged');
      }
      else {
        console.log('user is not logged');
      }
    });
  }

  public get currentUserValue() {
    return this.angularFireAuth.auth.currentUser;
  }

  /* Sign up */
  public signUp(email: string, password: string) {
  this.angularFireAuth.auth.createUserWithEmailAndPassword(email, password)
    .then(res => {
      console.log('You are Successfully signed up!', res);
    })
    .catch(error => {
      console.log('Something is wrong:', error.message);
    });
}

  /* Sign in */
  public signIn(email: string, password: string): Promise < firebase.auth.UserCredential > {
  return this.angularFireAuth.auth.signInWithEmailAndPassword(email, password);
}

  /* Sign out */
  public signOut() {
  this.angularFireAuth.auth.signOut()
    .then(_ => {
      console.log("Log out exitoso!");
      this.router.navigate(['/auth/login']);
    })
    .catch(err => {
      console.log("Error al salir: ", err);
    });
}

}

