const form = document.getElementById("vehicleData");

const formContainer = document.getElementsByClassName("form");

const selectElement = document.getElementById('vehicleYOM');
const currentYear = new Date().getFullYear();
const minYear = 2000;

// Generate the options dynamically
for (let year = currentYear; year >= minYear; year--) {
    const option = document.createElement('option');
    option.value = year;
    option.text = year;
    selectElement.appendChild(option);
}

//  array of vehicles in Kenya
const vehiclesArray = [
    'Toyota',
    'Nissan',
    'Honda',
    'Ford',
    'Mercedes',
    'Subaru',
    'Isuzu',
    'Mitsubishi',
    'Volkswagen',
    'LandRover',
    'BMW',
    'Lexus',
    'Mazda',
    'Hyundai',
    'Jeep',
    'Audi',
    'Peugeot',
    'Volvo',
    'Jaguar',
    'Suzuki',
    'Porsche'
];

// Sort the array alphabetically
const sortedVehicles = vehiclesArray.sort();

const selectVehicleMake = document.getElementById('vehicleMake');

for (let i = 0; i < sortedVehicles.length; i++) {
    const option = document.createElement('option');
    option.value = sortedVehicles[i];
    option.text = sortedVehicles[i];
    selectVehicleMake.appendChild(option);
}

const onSubmit = async (event) => {
    event.preventDefault();
    // console.log("submitting...");

    const firstName = document.getElementById("firstName").value;
    const lastName = document.getElementById("lastName").value;
    const email = document.getElementById("email").value;
    const gender = document.getElementById("gender").value;
    const phoneNumber = document.getElementById("phoneNumber").value;
    const whatsAppNumber = document.getElementById("whatsAppNumber").value;
    const batchNumber = document.getElementById("batchNumber").value;
    const serialNumber = document.getElementById("serialNumber").value;
    const registrationNumber = document.getElementById("registrationNumber").value;
    const dealerCode = document.getElementById("dealerCode").value;
    const vehicleMake = document.getElementById("vehicleMake").value;
    const vehicleModel = document.getElementById("vehicleModel").value;
    const productType = document.getElementById("productType").value;
    const transmissionType = document.getElementById("transmissionType").value;
    const vehicleYOM = document.getElementById("vehicleYOM").value;
    const carColor = document.getElementById("carColor").value;
    const chassisNumber = document.getElementById("chassisNumber").value;
    const fuelType = document.getElementById("fuelType").value;
    const engineCapacity = document.getElementById("engineCapacity").value;
    const engineCode = document.getElementById("engineCode").value;
    const mileageAtInstallation = document.getElementById("mileageAtInstallation").value;
    const mileageUnit = document.getElementById("mileageUnit").value;
    const batteryBrand = document.getElementById("batteryBrand").value;
    const tyreBrand = document.getElementById("tyreBrand").value;
    const networkMode = document.getElementById("networkMode").value;


    const vehicleEnrollData = {
        firstName,
        lastName,
        username: email,
        email,
        gender,
        telephone: phoneNumber,
        registrationNumber: "", //this is ID
        whatsappTelephone: whatsAppNumber,
        batchNumber: `SGL-${batchNumber}`,
        serialNumber: `SGL-${vehicleMake.toUpperCase()}-${serialNumber}`,
        vehicleRegistrationNumber: registrationNumber,
        dealerNumber: `WZ${dealerCode}`,
        vehicleMake,
        model: vehicleModel,
        productType,
        transmissionType,
        yearOfMake: vehicleYOM,
        vehicleTelephone: '',
        chasisNumber: chassisNumber,
        imei: '',
        fuelType,
        engineCapacity,
        engineCode,
        mileageAtInstallation,
        batteryBrand,
        carColor,
        mileageUnit,
        tyreBrand,
        networkMode
    };

    // console.log(vehicleEnrollData);

    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(vehicleEnrollData)
    };

    try {
        const response = await fetch("/submit", options);
        const data = await response.text();

        if (data == "Data received successfully") {
            formContainer[0].innerHTML = `
            <div class="success">
                <p>ScanWize Device Data for ${registrationNumber} has been saved successfully</p>
            </div>  
            `;
        } else {
            formContainer[0].innerHTML = `
            <div class="error">
                <p>Error: ${data}
                    Refresh page to input data again
                </p>
            </div>  
            `;
        }
        //console.log(data);
    } catch (error) {
        console.error(error);
    }
}

form.addEventListener('submit', onSubmit);

