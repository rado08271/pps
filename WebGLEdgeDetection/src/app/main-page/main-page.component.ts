import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-main-page',
  templateUrl: './main-page.component.html',
  styleUrls: ['./main-page.component.scss']
})
export class MainPageComponent implements OnInit {

  constructor() { }

  ngOnInit(): void {
  }

  url="";

  onSelectFile(event) {
    if(event.target.files) {
      let reader = new FileReader();
      reader.readAsDataURL(event.target.files[0]);

      reader.onload=(e:any) => {
        this.url = e.target.result;
        //console.log(this.url);
      }
    }
  }

  onDetectEdges(event) {
    //this.setCanvasSize();
    this.renderBackground();
  }

  setCanvasSize() {
    let image = document.getElementById("original-image");
    let canvas = document.getElementById("detected-image");
    canvas.style.width = image.offsetWidth + "px";
    canvas.style.height = image.offsetHeight + "px";
  }

  renderBackground() {
    if(this.url === "") {
      alert("You must upload a photo first!");
      return;
    }
    let canvas = document.querySelector("canvas");
    let gl = canvas.getContext("webgl");

    if (gl === null) {
      alert("Unable to initialize WebGL. Your browser or machine may not support it.");
      return;
    }
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
  }


}