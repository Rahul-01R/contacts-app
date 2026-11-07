// =====================================
// API CONFIGURATION
// =====================================

const USE_MOCK = false;

// Change this port according to Person C's server.
const API_BASE_URL = "http://localhost:8080/api";


// =====================================
// COMMON REQUEST FUNCTION
// =====================================

async function apiRequest(endpoint, options = {}) {

    try {

        const response = await fetch(
            `${API_BASE_URL}${endpoint}`,
            {
                ...options,

                headers: {
                    "Content-Type": "application/json",
                    ...options.headers
                }
            }
        );

        // Read server response
        const text = await response.text();

        let data = null;

        if (text) {

            try {
                data = JSON.parse(text);
            }
            catch {
                data = { message: text };
            }

        }

        // Handle HTTP errors
        if (!response.ok) {

            return {
                ok: false,
                status: response.status,
                data: data
            };

        }

        return {
            ok: true,
            status: response.status,
            data: data
        };

    }

    catch (error) {

        console.error("API Error:", error);

        return {
            ok: false,
            status: 0,
            data: {
                message: "Cannot connect to the server. Please check whether the backend is running."
            }
        };

    }

}


// =====================================
// GET ALL CONTACTS
// =====================================

async function getContacts(searchQuery = "") {

    let endpoint = "/contacts";

    if (searchQuery.trim() !== "") {

        endpoint += "?search=" + encodeURIComponent(searchQuery);

    }

    return await apiRequest(endpoint, {
        method: "GET"
    });

}


// =====================================
// CREATE CONTACT
// =====================================

async function createContact(contactData) {

    return await apiRequest("/contacts", {

        method: "POST",

        body: JSON.stringify(contactData)

    });

}


// =====================================
// UPDATE CONTACT
// =====================================

async function updateContact(id, updatedData) {

    return await apiRequest(`/contacts/${id}`, {

        method: "PUT",

        body: JSON.stringify(updatedData)

    });

}


// =====================================
// DELETE CONTACT
// =====================================

async function deleteContact(id) {

    return await apiRequest(`/contacts/${id}`, {

        method: "DELETE"

    });

}