import { Component, OnInit } from '@angular/core';
import { IRService } from '../../../services/ir.service';
import { ComandoIR } from '../../../models';
import { UtilService } from '../../../services';

@Component({
  selector: 'configuration-ir',
  templateUrl: './configuration-ir.component.html',
  styleUrls: ['./configuration-ir.component.scss']
})
export class ConfigurationIrComponent implements OnInit {
  comandosIR$;

  showInput = false;

  constructor(private irService: IRService, private utilService: UtilService) { }

  ngOnInit() {
    this.comandosIR$ = this.irService.getComandosIR();
  }

  guardarCambio(comandoIR, event: KeyboardEvent) {
    const nuevoComando = new ComandoIR(comandoIR);
    this.irService.updateComandoIR(comandoIR.key, nuevoComando)
    .then(res => {
      this.utilService.showToast('success', 'Cambios guardados satisfactoriamente!');
    })
    .catch(err => this.utilService.showToast('warning', 'Error al guardar los cambios', 'Es posible que se deba a un fallo en la comunicación', 4000))
  }

}
