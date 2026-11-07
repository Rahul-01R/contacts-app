// =========================================
// API CONFIGURATION
// =========================================

const API_BASE_URL = "http://127.0.0.1:8080/api";


// =========================================
// COMMON API REQUEST FUNCTION
// =========================================

async function apiRequest(endpoint, options = {}) {

    try {

        const response = await fetch(
            `${API_BASE_URL}${endpoint}`,
            {
                ...options,

                headers: {
                    "Accept": "application/json",
                    ...(options.body !== undefined
                        ? { "Content-Type": "application/json" }
                        : {}),
                    ...options.headers
                }
            }
        );


        // Read response

        const text = await response.text();

        let data = null;


        if (text) {

            try {

                data = JSON.parse(text);

            }

            catch {

                data = {
                    message: text
                };

            }

        }


        // Check HTTP status

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

        console.error("API Connection Error:", error);

        return {

            ok: false,

            status: 0,

            data: {

                message:
                    "Cannot connect to backend. Check whether the C++ server is running and CORS is enabled."

            }

        };

    }

}


// =========================================
// GET ALL CONTACTS
// =========================================

async function getContacts() {

    return await apiRequest("/contacts", {

        method: "GET"

    });

}


// =========================================
// SEARCH CONTACTS
// =========================================

async function searchContacts(query) {

    const endpoint =
        "/search?q=" + encodeURIComponent(query);

    return await apiRequest(endpoint, {

        method: "GET"

    });

}


// =========================================
// CREATE CONTACT
// =========================================

async function createContact(contactData) {

    return await apiRequest("/contacts", {

        method: "POST",

        body: JSON.stringify(contactData)

    });

}


// =========================================
// UPDATE CONTACT
// =========================================

async function updateContact(id, contactData) {

    return await apiRequest(
        `/contacts/${encodeURIComponent(id)}`,
        {

            method: "PUT",

            body: JSON.stringify(contactData)

        }
    );

}


// =========================================
// DELETE CONTACT
// =========================================

async function deleteContact(id) {

    return await apiRequest(
        `/contacts/${encodeURIComponent(id)}`,
        {

            method: "DELETE"

        }
    );

}


// =========================================
// CHECK BACKEND SERVER
// =========================================

async function checkServer() {

    return await apiRequest("/ping", {

        method: "GET"

    });

}