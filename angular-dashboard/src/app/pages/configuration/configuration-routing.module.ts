import { RouterModule, Routes } from '@angular/router';
import { NgModule } from '@angular/core';
import { ConfigurationComponent } from './configuration.component';
import { ConfigurationAulaComponent } from './configuration-aula/configuration-aula.component';
import { ConfigurationFirmwareComponent } from './configuration-firmware/configuration-firmware.component';
import { UrlRoutes } from '../../helpers';


const routes: Routes = [{
  path: '',
  component: ConfigurationComponent,
  children: [
    {
      path: '',
      component: ConfigurationAulaComponent,
    },
    // {
    //   path: 'nuevo-edificio',
    //   component: CreateEdificioComponent
    // },
    // {
    //   path: 'editar-edificio',
    //   component: EditEdificioComponent
    // },
    // {
    //   path: 'aulas',
    //   component: ListAulaComponent,
    // },
    // {
    //   path: 'nueva-aula',
    //   component: CreateAulaComponent
    // },
    // {
    //   path: 'editar-aula',
    //   component: EditAulaComponent
    // },
    {
      path: 'firmware',
      component: ConfigurationFirmwareComponent
    },
    {
        path: '',
        pathMatch: 'full',
        component: ConfigurationAulaComponent
    }
  ],
}];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class ConfigurationRoutingModule {
}
