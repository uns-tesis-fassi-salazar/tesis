import { Component, OnDestroy, Input, Output, EventEmitter, HostListener } from '@angular/core';
import { NbThemeService } from '@nebular/theme';
import { takeWhile } from 'rxjs/operators';

@Component({
  selector: 'ngx-sensor-display',
  styleUrls: ['./sensor-display.component.scss'],
  templateUrl: './sensor-display.component.html',
})

export class SensorDisplayComponent implements OnDestroy {

  @Input() title: string;
  // @Input() value: number;
  value = 50;
  @Input('value') set setValue(value) {
    this.value = value;
  }

  @Input() minValue: number;
  @Input() maxValue: number;
  @Input() unit_name: string;
  @Input() unit_symbol: string;
  @Input() displayOff: boolean;

  @Output() valueChange = new EventEmitter<Number>();

  isMouseDown = false;

  @HostListener('window:mouseup', ['$event'])
  onMouseUp(event) {
    if(this.isMouseDown && !this.displayOff){
      this.valueChange.emit(this.value);
      this.isMouseDown = false;
    }
  }

  mouseDown(event) {
    this.isMouseDown = true;
  }

  //temperature = 24;
  // temperatureOff = false;
  temperatureMode = 'cool';

  humidity = 87;
  humidityOff = false;
  humidityMode = 'heat';

  theme: any;
  themeSubscription: any;

  private alive = true;

  constructor(private themeService: NbThemeService) {
    this.themeService.getJsTheme()
      .pipe(takeWhile(() => this.alive))
      .subscribe(config => {
      this.theme = config.variables.temperature;
    });
  }

  ngOnDestroy() {
    this.alive = false;
  }

  onClick(){
    this.valueChange.emit(this.value);
  }
}


