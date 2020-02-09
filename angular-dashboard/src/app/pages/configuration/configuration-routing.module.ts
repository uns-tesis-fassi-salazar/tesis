import { RouterModule, Routes } from '@angular/router';
import { NgModule } from '@angular/core';
import { ConfigurationComponent } from './configuration.component';
import { ConfigurationFirmwareComponent } from './configuration-firmware/configuration-firmware.component';
import { UrlRoutes } from '../../helpers';
import { ConfigurationIrComponent } from './configuration-ir/configuration-ir.component';


const routes: Routes = [{
  path: '',
  component: ConfigurationComponent,
  children: [
    {
      path: 'comandos-ir',
      component: ConfigurationIrComponent
    },
    {
      path: 'firmware',
      component: ConfigurationFirmwareComponent
    }
  ],
}];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class ConfigurationRoutingModule {
}
