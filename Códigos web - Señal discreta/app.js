let señalR = [];
let señalG = [];
let señalB = [];

let tiempo = [];


const traces = [
    {
        x: tiempo,
        y: señalR,
        mode:'markers',
        name:'R',
        marker:{
            color:'red',
            size:12
        }
    },
    {
        x: tiempo,
        y: señalG,
        mode:'markers',
        name:'G',
        marker:{
            color:'lime',
            size:12
        }
    },

    {
        x: tiempo,
        y: señalB,
        mode:'markers',
        name:'B',
        marker:{
            color:'blue',
            size:12
        }
    }
];


const layout = {
    title:{
        text:'Señales Discretas RGB',
        font:{
            color:'white',
            size:28
        }
    },

    paper_bgcolor:'#0f172a',
    plot_bgcolor:'#111827',

    font:{
        color:'white'
    },

    xaxis:{
        title:'Tiempo n',
        gridcolor:'#334155',
        zerolinecolor:'#64748b'
    },

    yaxis:{
        title:'Amplitud RGB',
        range:[0,255],
        gridcolor:'#334155',
        zerolinecolor:'#64748b'
    }
};

Plotly.newPlot(
    'grafica',
    traces,
    layout,
    {
        responsive:true
    }
);


function actualizarStems(){
    let shapes = [];
    señalR.forEach((v,i)=>{
        shapes.push({
            type:'line',
            x0: tiempo[i],
            x1: tiempo[i],
            y0:0,
            y1:v,
            line:{
                color:'red',
                width:3
            }
        });
    });

    señalG.forEach((v,i)=>{
        shapes.push({
            type:'line',
            x0: tiempo[i],
            x1: tiempo[i],
            y0:0,
            y1:v,
            line:{
                color:'lime',
                width:3
            }
        });
    });


    señalB.forEach((v,i)=>{
        shapes.push({
            type:'line',
            x0: tiempo[i],
            x1: tiempo[i],
            y0:0,
            y1:v,
            line:{
                color:'blue',
                width:3
            }
        });
    });

    Plotly.relayout(
        'grafica',
        {
            shapes:shapes
        }
    );
}

document.getElementById("btn")
.addEventListener("click", async()=>{
    try{
        const port =
        await navigator.serial.requestPort();
        await port.open({
            baudRate:115200
        });
        document.getElementById("estado")
        .innerHTML =
        "🟢 Arduino conectado";

        const decoder =
        new TextDecoder();

        const reader =
        port.readable.getReader();

        let buffer = "";

        let n = 0;

        while(true){

            const { value, done } =
            await reader.read();

            if(done) break;

            buffer +=
            decoder.decode(value);

            let lineas =
            buffer.split("\n");

            buffer = lineas.pop();

            lineas.forEach(linea=>{

                linea = linea.trim();

                if(linea != ""){
                    const [r,g,b] =
                    linea.split(",");

                    let R = Number(r);
                    let G = Number(g);
                    let B = Number(b);

                    document
                    .getElementById("r")
                    .innerHTML =
                    "R = " + R;

                    document
                    .getElementById("g")
                    .innerHTML =
                    "G = " + G;

                    document
                    .getElementById("b")
                    .innerHTML =
                    "B = " + B;

                    document
                    .getElementById("colorBox")
                    .style.background =
                    `rgb(${R},${G},${B})`;

                    tiempo.push(n);
                    señalR.push(R);
                    señalG.push(G);
                    señalB.push(B);
                    n++;

                    if(tiempo.length > 20){

                        tiempo.shift();

                        señalR.shift();
                        señalG.shift();
                        señalB.shift();
                    }

                    Plotly.update(
                        'grafica',
                        {
                            x:[
                                tiempo,
                                tiempo,
                                tiempo
                            ],

                            y:[
                                señalR,
                                señalG,
                                señalB
                            ]
                        }
                    );
                    actualizarStems();
                }

            });

        }

    }catch(error){

        document.getElementById("estado")
        .innerHTML =
        "🔴 Error";

        console.log(error);
    }

});
