import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';

import {HttpClientModule} from "@angular/common/http";
import { SensorsService } from './sensors.service';

@NgModule({
  declarations: [
    AppComponent
  ],
  imports: [
    BrowserModule,
    HttpClientModule
  ],
  providers: [SensorsService],
  bootstrap: [AppComponent]
})
export class AppModule { }
