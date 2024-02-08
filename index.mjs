import express from "express";
import { PrismaClient } from "@prisma/client";
const app = express();

const prisma = new PrismaClient();

app.use(express.json());
app.use(express.urlencoded());

// Pages statiques
app.use(express.static("public"));

// Retourne tous les utilisateurs de la base
app.get("/devices", async (req, res) => {
  const allDevices = await prisma.device.findMany();
  res.status(200).json(allDevices);
});

app.get("/device/:id", async (req, res) => {
  const { id } = req.params;

  const device = await prisma.device.findUnique({
    where: { id: Number(id) },
  });
  res.json(device);
});


// Crée un nouvel utilisateur avec les données du formulaire
app.post("/new_device", async (req, res) => {
  const { nom, localisation } = req.body;
  const result = await prisma.device.create({
    data: {
      nom: nom,
      localisation: localisation,
    },
  });
  res.json(result);
});

// Lancement du serveur
app.listen(8080, () => {
  console.log("Serveur à l'écoute sur le port 8080");
});
