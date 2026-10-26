// ========================================
// 1. GET HTML ELEMENTS
// ========================================

const contactsContainer = document.getElementById("contacts-container");

const addContactButton = document.getElementById("addContact");

const searchInput = document.getElementById("search");

const contactDialog = document.getElementById("contactDialog");

const contactForm = document.getElementById("contactForm");

const dialogTitle = document.getElementById("dialogTitle");

const closeDialogButton = document.getElementById("closeDialog");

const cancelContactButton = document.getElementById("cancelContact");

const contactIdInput = document.getElementById("contactId");

const nameInput = document.getElementById("name");

const phoneInput = document.getElementById("phone");

const emailInput = document.getElementById("email");

const addressInput = document.getElementById("address");

const saveContactButton = document.getElementById("saveContact");


// ========================================
// 2. LOAD CONTACTS
// ========================================

let allContacts = [];


async function loadContacts() {
    try {
        allContacts = await getContacts();

        displayFilteredContacts();

    } catch (error) {
        console.error("Error loading contacts:", error);

        contactsContainer.replaceChildren();

        const message = document.createElement("p");

        message.className = "empty-message";

        message.textContent = "Unable to load contacts.";

        contactsContainer.appendChild(message);
    }
}


// ========================================
// 3. RENDER CONTACT CARDS
// ========================================

function renderContacts(list) {
    contactsContainer.replaceChildren();

    if (list.length === 0) {
        const message = document.createElement("p");

        message.className = "empty-message";

        message.textContent = "No contacts yet. Add your first one.";

        contactsContainer.appendChild(message);

        return;
    }


    list.forEach(contact => {

        // Main card

        const card = document.createElement("article");

        card.className = "contact-card";


        // Contact header

        const top = document.createElement("div");

        top.className = "contact-top";


        // Initials avatar

        const avatar = document.createElement("div");

        avatar.className = "contact-avatar";

        const initials = contact.name
            .trim()
            .split(/\s+/)
            .filter(Boolean)
            .slice(0, 2)
            .map(word => word[0].toUpperCase())
            .join("");

        avatar.textContent = initials || "?";


        // Contact name

        const heading = document.createElement("h3");

        heading.textContent = contact.name;


        const nameContainer = document.createElement("div");

        nameContainer.appendChild(heading);


        top.appendChild(avatar);

        top.appendChild(nameContainer);

        card.appendChild(top);


        // Phone number

        const phone = document.createElement("p");

        phone.textContent = "Phone: " + contact.phone;

        card.appendChild(phone);


        // Email address

        const email = document.createElement("p");

        email.textContent = "Email: " + (contact.email || "Not provided");

        card.appendChild(email);


        // Physical address

        const address = document.createElement("p");

        address.textContent = "Address: " + (contact.address || "Not provided");

        card.appendChild(address);


        // Action buttons

        const actions = document.createElement("div");

        actions.className = "contact-actions";


        // Edit button

        const editButton = document.createElement("button");

        editButton.type = "button";

        editButton.className = "edit-button";

        editButton.textContent = "Edit";

        editButton.addEventListener("click", () => {
            openEditDialog(contact);
        });


        // Delete button

        const deleteButton = document.createElement("button");

        deleteButton.type = "button";

        deleteButton.className = "delete-button";

        deleteButton.textContent = "Delete";

        deleteButton.addEventListener("click", () => {
            handleDeleteContact(contact.id);
        });


        actions.appendChild(editButton);

        actions.appendChild(deleteButton);

        card.appendChild(actions);


        // Add card to the page

        contactsContainer.appendChild(card);

    });
}


// ========================================
// 4. SEARCH CONTACTS
// ========================================

function displayFilteredContacts() {
    const query = searchInput.value.trim().toLowerCase();

    const filteredContacts = allContacts.filter(contact => {

        return (
            contact.name.toLowerCase().includes(query) ||
            contact.phone.toLowerCase().includes(query) ||
            contact.email.toLowerCase().includes(query) ||
            contact.address.toLowerCase().includes(query)
        );

    });

    renderContacts(filteredContacts);
}


searchInput.addEventListener("input", displayFilteredContacts);


// ========================================
// 5. CLEAR VALIDATION ERRORS
// ========================================

function clearErrors() {
    const fields = [
        nameInput,
        phoneInput,
        emailInput,
        addressInput
    ];

    const errorIds = [
        "nameError",
        "phoneError",
        "emailError",
        "addressError"
    ];


    fields.forEach(field => {
        field.classList.remove("invalid");

        field.removeAttribute("aria-invalid");
    });


    errorIds.forEach(id => {
        document.getElementById(id).textContent = "";
    });
}


// ========================================
// 6. DISPLAY A FIELD ERROR
// ========================================

function showError(field, errorId, message) {
    field.classList.add("invalid");

    field.setAttribute("aria-invalid", "true");

    document.getElementById(errorId).textContent = message;
}


// ========================================
// 7. VALIDATE THE FORM
// ========================================

function validateForm() {
    clearErrors();

    let isValid = true;


    // Get field values

    const name = nameInput.value.trim();

    const phone = phoneInput.value.trim();

    const email = emailInput.value.trim();


    // Validate name

    if (name === "") {
        showError(
            nameInput,
            "nameError",
            "Name must not be empty."
        );

        isValid = false;
    }


    // Validate phone number

    const digits = phone.replace(/\D/g, "");

    if (digits.length < 10) {
        showError(
            phoneInput,
            "phoneError",
            "Phone must contain at least 10 digits."
        );

        isValid = false;
    }


    // Validate email only when supplied

    if (
        email !== "" &&
        !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)
    ) {
        showError(
            emailInput,
            "emailError",
            "Enter a valid email address."
        );

        isValid = false;
    }


    return isValid;
}


// ========================================
// 8. OPEN THE ADD CONTACT DIALOG
// ========================================

function openAddDialog() {
    contactForm.reset();

    clearErrors();

    contactIdInput.value = "";

    dialogTitle.textContent = "Add Contact";

    saveContactButton.textContent = "Save Contact";

    contactDialog.showModal();

    nameInput.focus();
}


addContactButton.addEventListener("click", openAddDialog);


// ========================================
// 9. OPEN THE EDIT CONTACT DIALOG
// ========================================

function openEditDialog(contact) {
    contactForm.reset();

    clearErrors();


    // Store the ID of the selected contact

    contactIdInput.value = contact.id;


    // Fill in the existing details

    nameInput.value = contact.name;

    phoneInput.value = contact.phone;

    emailInput.value = contact.email;

    addressInput.value = contact.address;


    // Change the dialog title and button

    dialogTitle.textContent = "Edit Contact";

    saveContactButton.textContent = "Update Contact";


    // Open the dialog

    contactDialog.showModal();

    nameInput.focus();
}


// ========================================
// 10. CLOSE THE DIALOG
// ========================================

function closeContactDialog() {
    if (contactDialog.open) {
        contactDialog.close();
    }

    contactForm.reset();

    clearErrors();

    contactIdInput.value = "";
}


closeDialogButton.addEventListener(
    "click",
    closeContactDialog
);


cancelContactButton.addEventListener(
    "click",
    closeContactDialog
);


// ========================================
// 11. ADD OR UPDATE A CONTACT
// ========================================

contactForm.addEventListener("submit", async function (event) {

    // Prevent the page from refreshing

    event.preventDefault();


    // Stop if the form is invalid

    if (!validateForm()) {
        return;
    }


    // Collect the contact details

    const contactData = {
        name: nameInput.value.trim(),
        phone: phoneInput.value.trim(),
        email: emailInput.value.trim(),
        address: addressInput.value.trim()
    };


    // Check whether we are adding or editing

    const contactId = contactIdInput.value;


    // Prevent accidental repeated submissions

    saveContactButton.disabled = true;


    try {

        if (contactId === "") {

            // CREATE a new contact

            await createContact(contactData);

        } else {

            // UPDATE an existing contact

            await updateContact(contactId, contactData);

        }


        // Close the dialog after success

        closeContactDialog();


        // Reload and display the updated contacts

        await loadContacts();


    } catch (error) {

        console.error("Error saving contact:", error);

        alert("Unable to save the contact. Please try again.");

    } finally {

        saveContactButton.disabled = false;

    }

});


// ========================================
// 12. DELETE A CONTACT
// ========================================

async function handleDeleteContact(id) {

    // Ask for confirmation first

    const confirmed = window.confirm(
        "Are you sure you want to delete this contact?"
    );


    // Stop if the user cancels

    if (!confirmed) {
        return;
    }


    try {

        // Delete the selected contact

        await deleteContact(id);


        // Reload and display the updated contacts

        await loadContacts();


    } catch (error) {

        console.error("Error deleting contact:", error);

        alert("Unable to delete the contact. Please try again.");

    }

}


// ========================================
// 13. START THE APPLICATION
// ========================================

loadContacts();