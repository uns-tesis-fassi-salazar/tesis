import { NgModule } from '@angular/core';
import { NbLayoutModule, NbCardModule, NbButtonModule, NbAlertModule } from '@nebular/theme';
import { ThemeModule } from '../../@theme/theme.module';
import { MyAuthRoutingModule } from './auth-routing.module';

import { MyAuthComponent } from './auth.component';
import { LoginComponent } from './login/login.component';
import { ReactiveFormsModule, FormsModule } from '@angular/forms';
import { MiscellaneousModule } from '../miscellaneous/miscellaneous.module';
import { CommonModule } from '@angular/common';


@NgModule({
  imports: [
    MyAuthRoutingModule,
    ThemeModule,
    NbLayoutModule,
    MiscellaneousModule,
    ReactiveFormsModule,
    NbCardModule,
    NbButtonModule,
    NbAlertModule,
    // NbIconModule,
  ],
  declarations: [
    MyAuthComponent,
    LoginComponent
  ],
})
export class AuthModule { }

