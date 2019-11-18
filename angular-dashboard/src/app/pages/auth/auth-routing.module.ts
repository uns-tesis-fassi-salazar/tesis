import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { MyAuthComponent } from './auth.component';
import { LoginComponent } from './login/login.component';
import { NotFoundComponent } from '../miscellaneous/not-found/not-found.component';

const routes: Routes = [
  {
    path: '',
    component: MyAuthComponent,
    children: [
      {
        path: 'login',
        component: LoginComponent,
      },
      {
        path: '',
        redirectTo: 'login',
        pathMatch: 'full',
      },
      {
        path: '**',
        component: NotFoundComponent,
      },
    ],
  },
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class MyAuthRoutingModule { }
