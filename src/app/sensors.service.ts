import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

export class Sensor {
  name!: string;
  value!: number;
}

@Injectable({
  providedIn: 'root'
})
export class SensorsService {

  constructor(private http: HttpClient) { }

  public async getSensors(): Promise<Sensor[]> {
    return this.http.get<Sensor[]>('/sensors').toPromise();
  }

}
