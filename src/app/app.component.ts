import { Component } from '@angular/core';
import { Sensor, SensorsService } from './sensors.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'arduino-interface';

  sensors: Sensor[] = []

  constructor(private sensorsService: SensorsService) {
    setTimeout(this.getValues, 0, this);
  }

  getValues(component: AppComponent) {
    component.sensorsService.getSensors().then((val) => component.sensors = val);
    setTimeout(component.getValues, 1000, component);
  }

}
