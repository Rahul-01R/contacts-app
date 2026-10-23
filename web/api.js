// web/api.js

// Five sample contacts for Day 2
const sampleContacts = [
    {
        id: 1,
        name: "Rahul Kumar",
        phone: "9876543210",
        email: "rahul@gmail.com",
        address: "Visakhapatnam, Andhra Pradesh"
    },
    {
        id: 2,
        name: "Priya Sharma",
        phone: "9876501234",
        email: "priya@gmail.com",
        address: "Hyderabad, Telangana"
    },
    {
        id: 3,
        name: "Arjun Reddy",
        phone: "9123456780",
        email: "arjun@gmail.com",
        address: "Vijayawada, Andhra Pradesh"
    },
    {
        id: 4,
        name: "Sneha Patel",
        phone: "9988776655",
        email: "sneha@gmail.com",
        address: "Bengaluru, Karnataka"
    },
    {
        id: 5,
        name: "Kiran Kumar",
        phone: "9090909090",
        email: "kiran@gmail.com",
        address: "Chennai, Tamil Nadu"
    }
];

// Return the sample contacts
async function getContacts() {
    return sampleContacts;
}